###############
### IMPORTS ###
###############

import os
import argparse
import logging
import re
import numpy as np
import pandas as pd
import xarray as xr
import matplotlib.pyplot as plt
from geopy.distance import geodesic

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
        "file1",
        default=None,
        type=str,
        help="Path to the first GPS file you want to analyze",
    )
    parser.add_argument(
        "file2",
        default=None,
        type=str,
        help="Path to the second GPS file you want to plot/compare",
    )
    parser.add_argument(
        "--output_path",
        default=None,
        type=str,
        help="Path to the folder to save the .csv data into",
    )
    parser.add_argument(
        "--plot_files",
        default=False,
        type=bool,
        help="Choose to plot or not the two vehicles and their speed vectors on a map",
    )
    return parser


###############
### METHODS ###
###############


def parse_coordinates(coord_str):
    """
    Parses a coordinate string in the format 'DDMM.MMMM' (degrees, minutes, and optional decimal)
    with a hemisphere (N/S/E/W) and converts it to decimal degrees.

    Args:
    coord_str : str
        The coordinate string to parse (e.g., '12345.6789 N').

    Returns:
    decimal_degrees : float
        The corresponding decimal degrees.

    Raises:
    ValueError : If the coordinate string is in an invalid format.
    """
    coord = coord_str.split(":")[1].strip()
    # Extracts degrees and minutes using regex
    match = re.match(r"(\d{1,2})(\d{2})(?:\.(\d+))?\s([N|S|E|W])", coord)
    if not match:
        raise ValueError(f"Invalid coordinate format: {coord}")

    degrees = int(match.group(1))
    minutes = float(match.group(2)) / 60  # Convert minutes to decimal degrees
    decimal = float("." + match.group(3)) / 60 if match.group(3) else 0.0
    hemisphere = match.group(4)

    decimal_degrees = degrees + minutes + decimal
    if hemisphere in ["S", "W"]:
        decimal_degrees = -decimal_degrees

    return decimal_degrees


def load_and_process(file_path):
    """
    Loads and processes a CSV file or directory containing GPS data, extracting necessary fields
    and converting them into a DataFrame with appropriate formats.

    Args:
    file_path : str
        Path to a single CSV file or a directory containing CSV files with GPS data.

    Returns:
    pd.DataFrame
        A DataFrame containing the following columns:
        - 'DateTime': Combined date and time in datetime format.
        - 'Lat': Latitude in decimal degrees.
        - 'Lon': Longitude in decimal degrees.
        - 'Speed': Speed of the GPS unit.
    """
    if os.path.isfile(file_path):
        df = pd.read_csv(file_path)
    elif os.path.isdir(file_path):
        files = [
            file
            for file in os.listdir(file_path)
            if os.path.isfile(os.path.join(file_path, file)) and file.endswith(".csv")
        ]
        dataframes = []
        for file in files:
            df_temp = pd.read_csv(os.path.join(file_path, file))
            dataframes.append(df_temp)

        df = pd.concat(dataframes, ignore_index=True) if dataframes else pd.DataFrame()
    else:
        print(f"The file path provided {file_path} is unsupported.")
        return 0

    # Replace any year that starts with '00' to '20' to handle '0024' to '2024' conversions, etc.
    df["Date"] = df["Date"].str.replace(r"^00(\d{2})", r"20\1", regex=True)

    df["DateTime"] = pd.to_datetime(
        df["Date"] + " " + df["Heure"], format="%Y/%m/%d %H:%M:%S"
    )
    df.loc[df["Fix"] == "fix:1", "Lat"] = df.loc[df["Fix"] == "fix:1", "Lat"].apply(
        parse_coordinates
    )
    df.loc[df["Fix"] == "fix:1", "Lon"] = df.loc[df["Fix"] == "fix:1", "Lon"].apply(
        parse_coordinates
    )
    df["Speed"] = df["Speed"].str.replace("speed:", "")

    df = df.sort_values(by="DateTime").reset_index(drop=True)
    return df[["DateTime", "Lat", "Lon", "Speed"]]


def calculate_distances(df1, df2):
    """
    Calculates the distances between matching date-time entries from two GPS dataframes.

    Args:
    df1 : pd.DataFrame
        The first DataFrame containing GPS data with columns 'DateTime', 'Lat', 'Lon', and 'Speed'.

    df2 : pd.DataFrame
        The second DataFrame containing GPS data with columns 'DateTime', 'Lat', 'Lon', and 'Speed'.

    Returns:
    list of tuples
        A list of tuples where each tuple contains:
        - 'DateTime': The matched datetime.
        - 'Distance (meters)': The calculated distance between two points.
        - 'Lat file1', 'Lon file1': Coordinates of the first GPS file.
        - 'Lat file2', 'Lon file2': Coordinates of the second GPS file.
        - 'Speed file1', 'Speed file2': Speed values from both GPS files.
    """
    df_merged = pd.merge_asof(
        df1.sort_values("DateTime"),
        df2.sort_values("DateTime"),
        on="DateTime",
        direction="nearest",
        tolerance=pd.Timedelta("1min"),
    )
    df_merged = df_merged.dropna()

    distances = []
    for idx, row in df_merged.iterrows():
        coord1 = (row["Lat_x"], row["Lon_x"])
        coord2 = (row["Lat_y"], row["Lon_y"])
        distance = geodesic(coord1, coord2).meters
        distances.append(
            (
                row["DateTime"],
                distance,
                row["Lat_x"],
                row["Lon_x"],
                row["Lat_y"],
                row["Lon_y"],
                row["Speed_x"],
                row["Speed_y"],
            )
        )

    return distances


def get_coastline(limits):
    """
    Retrieves the bathymetric coastline data for a given geographical limit range.

    Args:
    limits : list
        A list containing the geographical limits [min_lat, min_lon, max_lat, max_lon].

    Returns:
    xarray.Dataset
        A dataset containing the interpolated bathymetric data for the specified limits.
    """
    bathy_dataset = xr.open_dataset(
        "/CIAN/DATA/GEBCO_WORLD/worldwide/GEBCO_2023_sub_ice_topo.nc"
    )
    lon_range = np.arange(limits[1], limits[3], 0.0001)
    lat_range = np.arange(limits[0], limits[2], 0.0001)
    lon_grid, lat_grid = np.meshgrid(lon_range, lat_range)

    coastline = bathy_dataset.interp(
        lon=xr.DataArray(lon_grid, dims=["y", "x"]),
        lat=xr.DataArray(lat_grid, dims=["y", "x"]),
        method="nearest",
    )

    return coastline


def plot_positions(distance_df, coastline, limits, date, args):
    """
    Plots the GPS positions of two files overlaid on bathymetric data and saves the plot.

    Args:
    distance_df : pd.DataFrame
        DataFrame containing GPS data with columns: 'DateTime', 'Distance (meters)',
        'Lat file1', 'Lon file1', 'Lat file2', 'Lon file2', 'Speed file1', 'Speed file2'.
    coastline : xarray.Dataset
        Dataset containing the bathymetric coastline data.
    limits : list
        List of geographical limits [min_lat, min_lon, max_lat, max_lon].
    date : str
        Date string used in the plot title.
    args : argparse.Namespace
        Command line arguments containing output path for saving the plot.
    """
    plt.figure()

    plt.figure()
    plt.imshow(
        coastline["elevation"],  # Use bathymetric elevation data
        extent=[
            limits[1],
            limits[3],
            limits[0],
            limits[2],
        ],  # longitude and latitude limits
        cmap="terrain",  # A color map for bathymetric data
        origin="lower",  # Ensures correct orientation
        aspect="auto",  # Allows the map to auto-scale
        vmax=0,
    )
    plt.colorbar(label="Elevation (m)")

    plt.scatter(
        distance_df["Lon file1"],
        distance_df["Lat file1"],
        c=distance_df["DateTime"],
        cmap="jet",
        label="File 1 Positions",
        alpha=0.5,
        marker="+",
    )
    if "Lon file2" in distance_df.columns and "Lat file2" in distance_df.columns:
        plt.scatter(
            distance_df["Lon file2"],
            distance_df["Lat file2"],
            c=distance_df["DateTime"],
            cmap="jet",
            label="File 2 Positions",
            alpha=0.5,
            marker="o",
        )

    # for idx in range(len(distance_df) - 1):
    #     plt.arrow(
    #         distance_df["Lon file1"].iloc[idx],
    #         distance_df["Lat file1"].iloc[idx],
    #         float(distance_df["Speed file1"].iloc[idx])
    #         * 0.0001
    #         * (
    #             distance_df["Lon file1"].iloc[idx + 1]
    #             - distance_df["Lon file1"].iloc[idx]
    #         ),
    #         float(distance_df["Speed file1"].iloc[idx])
    #         * 0.0001
    #         * (
    #             distance_df["Lat file1"].iloc[idx + 1]
    #             - distance_df["Lat file1"].iloc[idx]
    #         ),
    #         color="blue",
    #         alpha=0.5,
    #         head_width=0.002,
    #     )

    #     plt.arrow(
    #         distance_df["Lon file2"].iloc[idx],
    #         distance_df["Lat file2"].iloc[idx],
    #         float(distance_df["Speed file2"].iloc[idx])
    #         * 0.0001
    #         * (
    #             distance_df["Lon file2"].iloc[idx + 1]
    #             - distance_df["Lon file2"].iloc[idx]
    #         ),
    #         float(distance_df["Speed file2"].iloc[idx])
    #         * 0.0001
    #         * (
    #             distance_df["Lat file2"].iloc[idx + 1]
    #             - distance_df["Lat file2"].iloc[idx]
    #         ),
    #         color="red",
    #         alpha=0.5,
    #         head_width=0.002,
    #     )

    plt.title(f"GPS Positions with bathymetric data \n {date}")
    plt.xlabel("Longitude")
    plt.ylabel("Latitude")
    plt.legend()
    plt.grid()
    plt.axis("scaled")
    print(os.path.join(
            args.output_path,
            f"GPS_Plot_{os.path.splitext(os.path.basename(args.file1))[0]}{date}.png",
        ))
    plt.savefig(
        os.path.join(
            args.output_path,
            f"GPS_Plot_{os.path.splitext(os.path.basename(args.file1))[0]}{date}.png",
        )
    )  # Save the plot
    plt.show()


############
### MAIN ###
############


def main(args):
    """
    Main function to calculate distances between matching date-time entries in two GPS files,
    plot the positions of the USVs, and save the results to a CSV and image file.

    Args:
    args : argparse.Namespace
        Command line arguments containing input file paths, output path, and plot flag.
        - args.file1: Path to the first GPS data file.
        - args.file2: Path to the second GPS data file.
        - args.output_path: Path to save the output files.
        - args.plot_files: Boolean flag to determine if plots should be generated.
    """
    if args.output_path is None:
        args.output_path = os.path.dirname(args.file1)

    full_date_match = re.search(
        r"(\d{4})(?:-?)(\d{2})(?:-?)(\d{2})[_](\d{4})(?:-?)(\d{2})(?:-?)(\d{2})",
        args.file1,
    ).group(0)

    data1 = load_and_process(args.file1)
    data2 = load_and_process(args.file2)

    min_lat = min(data1["Lat"].min(skipna=True), data2["Lat"].min(skipna=True))
    min_lon = min(data1["Lon"].min(skipna=True), data2["Lon"].min(skipna=True))
    max_lat = max(data1["Lat"].max(skipna=True), data2["Lat"].max(skipna=True))
    max_lon = max(data1["Lon"].max(skipna=True), data2["Lon"].max(skipna=True))
    lat_scale = 0.2 * (max_lat - min_lat)
    lon_scale = 0.2 * (max_lon - min_lon)

    limits = [
        min_lat - lat_scale,
        min_lon - lon_scale,
        max_lat + lat_scale,
        max_lon + lon_scale,
    ]

    distances = calculate_distances(data1, data2)
    # for dt, dist, _, _, _, _, _, _ in distances:
    # print(f"DateTime: {dt}, Distance (meters): {dist:.2f}")

    # Convert to DataFrame and save as CSV
    distances_df = pd.DataFrame(
        distances,
        columns=[
            "DateTime",
            "Distance (meters)",
            "Lat file1",
            "Lon file1",
            "Lat file2",
            "Lon file2",
            "Speed file1",
            "Speed file2",
        ],
    )
    distances_df[["DateTime", "Distance (meters)"]].to_csv(
        os.path.join(
            args.output_path,
            f"Distance_Table_{os.path.splitext(os.path.basename(args.file1))[0]}_{os.path.splitext(os.path.basename(args.file2))[0]}.csv",
        ),
        index=False,
    )

    if args.plot_files:
        coastline = get_coastline(limits)
        # Plot positions and speed vectors
        plot_positions(distances_df, coastline, limits, full_date_match, args)


if __name__ == "__main__":
    args = get_args_parser().parse_args()
    try:
        # Set up logging configuration
        logging.basicConfig(
            level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
        )

        # Call main function
        main(args)
    except Exception as e:
        logging.exception(f"Unhandled exception: {e}")
