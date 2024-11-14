###############
### IMPORTS ###
###############

import re
import argparse
import os
import pandas as pd
from gps_to_dist import get_coastline, plot_positions, parse_coordinates

###############
### METHODS ###
###############


def pps_gps_data_extraction(qhb_csv):
    """
    Extracts GPS and PPS data from a DataFrame and returns separate DataFrames for each.

    Args:
    qhb_csv : pd.DataFrame
        Input DataFrame containing GPS and PPS sensor data with columns "Sensor Type",
        "TimeStamp(ms) or Time", and relevant sensor data fields.

    Returns:
    pps_data : pd.DataFrame
        DataFrame containing PPS data with a single column "Timestamp" derived
        from the "Sensor Type" column in the input.
    gps_data : pd.DataFrame
        DataFrame containing GPS data with columns:
            - "Date": Extracted and formatted date in the format "YYYY/MM/DD"
            - "Heure": Extracted and formatted time in the format "HH:MM:SS"
            - "Fix": Fix status extracted from the timestamp
            - "Fix Qual": Quality of the fix
            - "Lat": Latitude
            - "Lon": Longitude
            - "Speed": Speed
            - "Angle": Angle
            - "Alt": Altitude
            - "Sat": Satellite count
        GPS data is only extracted if "Sensor Type" is "GPS" and the fix status is "fix:1".
    """
    gps_data = pd.DataFrame()
    pps_data = pd.DataFrame()
    for _, row in qhb_csv.iterrows():
        sensor_type = row["Sensor Type"]
        if (
            sensor_type == "GPS"
            and str(row["TimeStamp(ms) or Time"]).split(" ")[3] == "fix:1"
        ):
            gps_data = pd.concat(
                [
                    gps_data,
                    pd.DataFrame(
                        {
                            "Date": f"20{int(str(row['TimeStamp(ms) or Time']).split(' ')[1].split('/')[0]):02}/{int(str(row['TimeStamp(ms) or Time']).split(' ')[1].split('/')[1]):02}/{int(str(row['TimeStamp(ms) or Time']).split(' ')[1].split('/')[2]):02}",
                            "Heure": f"{int(str(row['TimeStamp(ms) or Time']).split(' ')[2].split(':')[0]):02}:{int(str(row['TimeStamp(ms) or Time']).split(' ')[2].split(':')[1]):02}:{int(str(row['TimeStamp(ms) or Time']).split(' ')[2].split(':')[2]):02}",
                            "Fix": str(row["TimeStamp(ms) or Time"]).split(" ")[3],
                            "Fix Qual": row[" val0"],
                            "Lat": row["val1"],
                            "Lon": row["val2"],
                            "Speed": row["val3"],
                            "Angle": row["val4"],
                            "Alt": row["val5"],
                            "Sat": row["val6"],
                        },
                        index=["Date"],
                    ),
                ],
                ignore_index=True,
            )
        elif "PPS" in sensor_type:
            pps_data = pd.concat(
                [
                    pps_data,
                    pd.DataFrame(
                        {
                            "Timestamp": str(row["Sensor Type"]).split(":")[1],
                        },
                        index=["Timestamp"],
                    ),
                ],
                ignore_index=True,
            )

    return pps_data, gps_data


def write_data(output_path, file, pps_data, gps_data):
    """Write out the extracted data to dedicated .csv files

    Args:
        output_path (str): Path to the output directory
        file (str): Name of the file the data has been extracted from
        pps_data (DataFrame): DataFrame containing the extracted pps data
        gps_data (DataFrame): DataFrame containing the extracted gps data
    """
    pps_data.to_csv(
        os.path.join(
            output_path,
            "PPS_"
            + os.path.basename(os.path.dirname(output_path))
            + "_"
            + os.path.splitext(os.path.basename(file))[0]
            + ".csv",
        ),
        index=False,
    )
    gps_data.to_csv(
        os.path.join(
            output_path,
            "GPS_"
            + os.path.basename(os.path.dirname(output_path))
            + "_"
            + os.path.splitext(os.path.basename(file))[0]
            + ".csv",
        ),
        index=False,
    )


def extract_datetime(filename):
    """Extract the date and time based on three possible formats depending on the separators : ["_", "-", ""]

    Args:
        filename (str): Name of the file to extract the date and time from

    Returns:
        str: Concatenation of the string representing the date and the string representing the time
    """
    # Extract date
    full_date_match = re.search(
        r"(\d{4})(?:-?)(\d{2})(?:-?)(\d{2})[_](\d{2})(?:-?)(\d{2})(?:-?)(\d{2})",
        filename,
    )
    full_date_str = full_date_match.group(0)
    date_str = full_date_str.split("_")[0]
    time_str = full_date_str.split("_")[1]

    return date_str + time_str


def save_plot_pipeline(gps_data, file, args):
    full_date_match = re.search(
        r"(\d{4})(?:-?)(\d{2})(?:-?)(\d{2})[_](\d{2})(?:-?)(\d{2})(?:-?)(\d{2})",
        file,
    ).group(0)
    
    gps_data["Date"] = gps_data["Date"].str.replace(r"^00(\d{2})", r"20\1", regex=True)
    gps_data["DateTime"] = pd.to_datetime(
        gps_data["Date"] + " " + gps_data["Heure"], format="%Y/%m/%d %H:%M:%S"
    )
    gps_data.loc[gps_data["Fix"] == "fix:1", "Lat"] = gps_data.loc[gps_data["Fix"] == "fix:1", "Lat"].apply(
        parse_coordinates
    )
    gps_data.loc[gps_data["Fix"] == "fix:1", "Lon"] = gps_data.loc[gps_data["Fix"] == "fix:1", "Lon"].apply(
        parse_coordinates
    )
    gps_data["Speed"] = gps_data["Speed"].str.replace("speed:", "")
    gps_data = gps_data.sort_values(by="DateTime").reset_index(drop=True)

    min_lat = gps_data["Lat"].min(skipna=True)
    min_lon = gps_data["Lon"].min(skipna=True)
    max_lat = gps_data["Lat"].max(skipna=True)
    max_lon = gps_data["Lon"].max(skipna=True)
    lat_scale = 0.2 * (max_lat - min_lat)
    lon_scale = 0.2 * (max_lon - min_lon)

    limits = [
        min_lat - lat_scale,
        min_lon - lon_scale,
        max_lat + lat_scale,
        max_lon + lon_scale,
    ]

    coastline = get_coastline(limits)

    gps_data = gps_data.rename(columns={"Lat": "Lat file1", "Lon": "Lon file1"})
    args.file1 = file
    plot_positions(gps_data, coastline, limits, full_date_match, args)


########################
### ARGUMENTS PARSER ###
########################


def get_args_parser(add_help=True):
    """Create an argument parser for command-line options.

    Parameters:
        add_help (bool): Whether to add the help option to the parser. Default is True.

    Returns:
        argparse.ArgumentParser: Configured argument parser with defined arguments.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "input_path",
        default=None,
        type=str,
        help="Path to the input IMU file or directory you want to extract GPS from",
    )
    parser.add_argument(
        "--output_path",
        default=None,
        type=str,
        help="Path to the output directory you want to save file(s) to",
    )
    parser.add_argument(
        "--save_plot",
        default=False,
        type=bool,
        help="For plotting and saving GPS trace",
    )
    parser.add_argument(
        "--merge_data",
        default=False,
        type=bool,
        help="For merging the several IMU files into one trace",
    )
    return parser


############
### MAIN ###
############


def main(args):
    """Main function of this script.

    Args:
        args (argument parser): Arguments passed to run the script.
    """
    if os.path.isdir(args.input_path):
        if args.output_path is None:
            args.output_path = args.input_path
        qhb_csv = pd.DataFrame()

        files = sorted(
            [f for f in os.listdir(args.input_path) if f.endswith(".csv")],
            key=(extract_datetime),
        )
        for file in files:
            if args.merge_data:
                try:
                    qhb_csv = pd.concat(
                        [
                            qhb_csv,
                            pd.read_csv(
                                os.path.join(args.input_path, file),
                                on_bad_lines="skip",
                            ),
                        ],
                        ignore_index=True,
                    )
                except Exception as e:
                    print(f"Error processing file {file}: {e}")
                continue
            qhb_csv = pd.read_csv(os.path.join(args.input_path, file))
            pps_data, gps_data = pps_gps_data_extraction(qhb_csv)
            write_data(args.output_path, file, pps_data, gps_data)

            if args.save_plot:
                save_plot_pipeline(gps_data, file, args)

        file = files[0]
        if not args.merge_data:
            return 0

    else:
        if args.output_path is None:
            args.output_path = os.path.dirname(args.input_path)
        qhb_csv = pd.read_csv(args.input_path)
        file = args.input_path
        print(qhb_csv)

    pps_data, gps_data = pps_gps_data_extraction(qhb_csv)
    write_data(args.output_path, args.input_path, pps_data, gps_data)

    if args.save_plot:
        save_plot_pipeline(gps_data, file, args)


if __name__ == "__main__":
    args = get_args_parser().parse_args()
    main(args)
