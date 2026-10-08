import tables as tb
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import pyvista as pv
import pathlib
import pandas as pd


def data_reading(path, table):
    with tb.open_file(path, 'r') as file:
        data = file.root.MC.__getitem__(table).read()
        # print(data.shape)
        column_names = file.root.MC.__getitem__(table).colnames

    dataframe = pd.DataFrame(data=data, columns=column_names)
    return dataframe


def data_reading_wo_pd(path, table):
    with tb.open_file(path, 'r') as file:
            data = file.root.MC.__getitem__(table).read()
        #     print(data)
            column_names = file.root.MC.__getitem__(table).colnames
    return data, column_names


def sipm_position(copy_no, n_sipm_side=10, sipm_size=6.0, sipm_gap=0.2,  z=79.0):#the z position is computed for the cylinder
    """
    Return the (x, y) center position of a SiPM in mm,
    given its Geant4 copy number.
    """
    pitch = sipm_size + sipm_gap

    odd = n_sipm_side % 2 == 1
    start = -(n_sipm_side // 2) * pitch
    start += 0.5 * pitch if not odd else 0.0

    i = copy_no // n_sipm_side
    j = copy_no % n_sipm_side

    x = i * pitch + start
    y = j * pitch + start

    return x, y, z





def plot_tracks(path, n_events=None, seed=None):
    """
    Reconstruct and plot the trajectories from sources to SiPM hits.

    Parameters
    ----------
    path : str
        Path to the HDF5 file.
    n_events : int, optional
        Number of events to select randomly. If None, all events are plotted.
    seed : int, optional
        Random seed for reproducibility.

    Returns
    -------
    fig, ax
        Matplotlib figure and 3D axes.
    """

    # Read tables
    sources = data_reading(path, "sources")
    sensor_hits = data_reading(path, "sensor_hits")

    # Select random events
    if n_events is not None:

        available_events = sources["event"].unique()

        if n_events > len(available_events):
            raise ValueError(
                f"Requested {n_events} events, "
                f"but only {len(available_events)} are available."
            )

        rng = np.random.default_rng(seed)

        selected_events = rng.choice(
            available_events,
            size=n_events,
            replace=False
        )

        sources = sources[
            sources["event"].isin(selected_events)
        ]

        sensor_hits = sensor_hits[
            sensor_hits["event"].isin(selected_events)
        ]

    # Create figure
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection="3d")

    # Group sensor hits by event
    hits_by_event = sensor_hits.groupby("event")

    # Loop over events
    for event, source_event in sources.groupby("event"):

        # One source point per event
        source = source_event.iloc[0]

        x0 = source["x"]
        y0 = source["y"]
        z0 = source["z"]

        # Plot source
        ax.scatter(
            x0, y0, z0,
            marker="*",
            s=100
        )

        # Skip events without sensor hits
        if event not in hits_by_event.groups:
            continue

        hits_event = hits_by_event.get_group(event)
        
        if len(hits_event) > 10:
            hits_event = hits_event.sample(n=10, random_state=seed)
        else:
            hits_event = hits_event.sample(n=len(hits_event), random_state=seed)

        # Draw trajectory from source to each SiPM hit
        for _, hit in hits_event.iterrows():

            sensor_id = int(hit["sensor_id"])

            x1, y1, z1 = sipm_position(sensor_id)

            ax.plot(
                [x0, x1],
                [y0, y1],
                [z0, z1],
                alpha=0.4
            )

            # Plot SiPM hit
            ax.scatter(
                x1, y1, z1,
                s=10
            )

    ax.set_xlabel("X [mm]")
    ax.set_ylabel("Y [mm]")
    ax.set_zlabel("Z [mm]")

    ax.set_title("Reconstructed particle trajectories")

    plt.tight_layout()
    # plt.show()

    return fig, ax



def plot_sources(path, n_events=None, seed=None):
    """
    Plot source positions in the X-Y plane.

    Parameters
    ----------
    path : str
        Path to the HDF5 file.
    n_events : int, optional
        Number of events to select randomly. If None, all events are plotted.
    seed : int, optional
        Random seed for reproducibility.

    Returns
    -------
    fig, ax
        Matplotlib figure and axes.
    """

    # Read sources
    sources = data_reading(path, "sources")

    # Select random events
    if n_events is not None:

        available_events = sources["event"].unique()

        if n_events > len(available_events):
            raise ValueError(
                f"Requested {n_events} events, "
                f"but only {len(available_events)} are available."
            )

        rng = np.random.default_rng(seed)

        selected_events = rng.choice(
            available_events,
            size=n_events,
            replace=False
        )

        sources = sources[
            sources["event"].isin(selected_events)
        ]

    # Plot
    fig, ax = plt.subplots(figsize=(8, 8))

    ax.scatter(
        sources["x"],
        sources["y"],
        s=10
    )

    ax.set_xlabel("X [mm]")
    ax.set_ylabel("Y [mm]")
    ax.set_title("Source positions")

    ax.set_aspect("equal")

    plt.tight_layout()
    plt.show()

    return fig, ax





def plot_event_hits(path, event):
    data_sensors, _ = data_reading_wo_pd(path, 'sensor_hits')
    data_sources, _ = data_reading_wo_pd(path, 'sources')

    # print(data_sources)

    #Source position----------------------------------------------
    source_mask = data_sources['event'] == event

    positions = np.column_stack([
        data_sources['x'],
        data_sources['y'],
        data_sources['z']
    ])

    source_pos = positions[source_mask]
    print(source_pos)


    #Hits positions-----------------------------------------------
    hits_mask = data_sensors['event'] == event

    # print(len(data_sensors['sensor_id'][hits_mask]))

    sens_ids = data_sensors['sensor_id'][hits_mask]

    sens_poss= np.array([sipm_position(sens_id) for sens_id in sens_ids])
    print(np.shape(sens_poss))

    x_hits = sens_poss[:, 0]
    y_hits = sens_poss[:, 1]

    # sipm_ids = [[i*10 + j for j in range(10)] for i in range(10)]
    sipm_x_poss = [sipm_position(i)[0] for i in range(100)]
    sipm_y_poss = [sipm_position(j)[1] for j in range(100)]



    # Histograma 2D
    hist, xedges, yedges = np.histogram2d( x_hits, y_hits, bins=10 )

    fig, ax = plt.subplots()

    im = ax.imshow( hist.T, origin='lower', extent=[ sipm_x_poss[0], sipm_x_poss[-1], sipm_y_poss[0], sipm_y_poss[-1] ], aspect='equal' )

    ax.set_xlabel("x [mm]")
    ax.set_ylabel("y [mm]")

    fig.colorbar(im, ax=ax, label="Number of hits")

    ax.plot(source_pos[0][0], source_pos[0][1], marker='o', color='white', label='Source')
    ax.legend()



def n_evs_vs_R(path):
    data_sensors, _ = data_reading_wo_pd(path, 'sensor_hits')
    data_sources, _ = data_reading_wo_pd(path, 'sources')


    positions = np.column_stack([
            data_sources['x'],
            data_sources['y'],
            data_sources['z']
        ])
    
    Rs = []
    n_hits_list = []
    for event in data_sources['event']:
        source_mask = data_sources['event'] == event
        x = data_sources[source_mask]['x']
        y = data_sources[source_mask]['y']

        R = np.sqrt(x ** 2 + y ** 2)
        Rs.append(R)


        hits_mask = data_sensors['event'] == event
        n_hits = len(data_sensors['sensor_id'][hits_mask])
        n_hits_list.append(n_hits)
        if event > 10000:
            break

    plt.plot(Rs, n_hits_list, '.')