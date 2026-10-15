from sys import argv


def parse_csv(filename: str):
    with open(filename) as f:
        data = f.read().split("\n")
        data = data[1:len(data)-1]

    data = list(map(lambda e: e.split(","), data))

    data = list(map(lambda e: (float(e[0]), float(e[1]), float(e[2])), data))

    return data

def matplotlib_visualisation(sensor1_data, sensor2_data):
    import matplotlib.pyplot as plt
    plt.style.use('_mpl-gallery')
    # Plot sensor1
    timestamps = [p[0] for p in sensor1_data]
    x = [p[1] for p in sensor1_data]
    y = [p[2] for p in sensor1_data]

    fig = plt.figure(dpi=150, figsize=(10, 6))
    ax = fig.add_subplot(111, projection="3d")

    ax.scatter3D(x, y, timestamps)

    sc = ax.scatter(
        x, y, timestamps,
        c=timestamps,
        cmap="viridis",
        s=12
    )

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Timestamp")

    fig.colorbar(sc, ax=ax, label="Timestamp", pad=0.1)
    plt.tight_layout()

    ax.set(xticklabels=[],
           yticklabels=[],
           zticklabels=[])

    plt.savefig("sensor1.png")
    plt.show()


    # Plot sensor2
    timestamps = [p[0] for p in sensor2_data]
    x = [p[1] for p in sensor2_data]
    y = [p[2] for p in sensor2_data]

    fig = plt.figure(dpi=150, figsize=(10, 6))
    ax = fig.add_subplot(111, projection="3d")

    ax.scatter3D(x, y, timestamps)

    sc = ax.scatter(
        x, y, timestamps,
        c=timestamps,
        cmap="viridis",
        s=12
    )

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Timestamp")

    fig.colorbar(sc, ax=ax, label="Timestamp", pad=0.1)
    plt.tight_layout()

    ax.set(xticklabels=[],
           yticklabels=[],
           zticklabels=[])

    plt.savefig("sensor2.png")
    plt.show()


def plotly_visualization(data, name):
    import plotly.graph_objects as go

    timestamps = [p[0] for p in data]
    x = [p[1] for p in data]
    y = [p[2] for p in data]

    fig = go.Figure(
        data=[
            go.Scatter3d(
                x=x,
                y=y,
                z=timestamps,
                mode="markers",
                marker=dict(
                    size=3,
                    color=timestamps,
                    colorscale="Viridis",
                    colorbar=dict(title="Time"),
                    opacity=0.8
                ),
                hovertemplate=(
                    "X: %{x}<br>"
                    "Y: %{y}<br>"
                    "Timestamp: %{z}"
                    "<extra></extra>"
                )
            )
        ]
    )

    fig.update_layout(
        title="3D Trajectory",
        scene=dict(
            xaxis_title="X",
            yaxis_title="Y",
            zaxis_title="Timestamp",
        ),
        width=1000,
        height=800,
    )

    fig.write_html(name)


def calc_pos_delta(sensor1_data, sensor2_data):
    # interpolation in the lower frequency set
    def get_pos(timestamp):
        for i in range(len(sensor2_data)-1):
            t0, x0, y0 = sensor2_data[i]
            t1, x1, y1 = sensor2_data[i+1]
            if t0 <= timestamp <= t1:
                alpha = (timestamp - t0) / (t1 - t0)
                x = x0 + alpha * (x1 - x0)
                y = y0 + alpha * (y1 - y0)
                return x,y
        return sensor2_data[0][1], sensor2_data[0][2]
    pos_delta: list = []

    for e in sensor1_data:
        pos_delta.append((e[0], get_pos(e[0])[0] - e[1], get_pos(e[0])[1] - e[2]))
    return pos_delta

def interpolation_visualization2(sensor2_data):
    import plotly.graph_objects as go

    original_t = [p[0] for p in sensor2_data]
    original_x = [p[1] for p in sensor2_data]
    original_y = [p[2] for p in sensor2_data]

    # Generate interpolated points between every pair of real points
    interpolated_t = []
    interpolated_x = []
    interpolated_y = []

    points_per_segment = 3  # Number of interpolated points between real points

    for i in range(len(sensor2_data) - 1):
        t0, x0, y0 = sensor2_data[i]
        t1, x1, y1 = sensor2_data[i + 1]

        for j in range(1, points_per_segment + 1):
            # Same linear interpolation as in position_interpolator.cpp

            alpha = j / (points_per_segment + 1)

            timestamp = t0 + alpha * (t1 - t0)

            x = x0 + alpha * (x1 - x0)
            y = y0 + alpha * (y1 - y0)

            interpolated_t.append(timestamp)
            interpolated_x.append(x)
            interpolated_y.append(y)

    fig = go.Figure()

    # Original trajectory, connecting real points
    fig.add_trace(go.Scatter3d(
        x=original_x,
        y=original_y,
        z=original_t,
        mode="lines+markers",
        name="Original sensor2 points",
        line=dict(color="royalblue", width=4),
        marker=dict(size=5, color="royalblue"),
        hovertemplate=(
            "Original point"
            "<br>Timestamp: %{z:.6f}"
            "<br>X: %{x:.6f}"
            "<br>Y: %{y:.6f}"
            "<extra></extra>"
        )
    ))

    # Interpolated positions
    fig.add_trace(go.Scatter3d(
        x=interpolated_x,
        y=interpolated_y,
        z=interpolated_t,
        mode="markers",
        name="Interpolated points",
        marker=dict(
            size=4,
            color="darkorange",
            symbol="diamond",
            opacity=0.9
        ),
        hovertemplate=(
            "Interpolated point"
            "<br>Timestamp: %{z:.6f}"
            "<br>X: %{x:.6f}"
            "<br>Y: %{y:.6f}"
            "<extra></extra>"
        )
    ))

    fig.update_layout(
        title="Sensor 2 — Linear Interpolation Visualization",
        scene=dict(
            xaxis_title="X",
            yaxis_title="Y",
            zaxis_title="Timestamp",
        ),
        width=1100,
        height=850,
        legend=dict(x=0.02, y=0.98),
    )

    fig.write_html("interpolation2.html")

def interpolation_visualization1(sensor1_data):
    import plotly.graph_objects as go

    original_t = [p[0] for p in sensor1_data]
    original_x = [p[1] for p in sensor1_data]
    original_y = [p[2] for p in sensor1_data]

    # Generate interpolated points between every pair of real points
    interpolated_t = []
    interpolated_x = []
    interpolated_y = []

    points_per_segment = 3  # Number of interpolated points between real points

    for i in range(len(sensor1_data) - 1):
        t0, x0, y0 = sensor1_data[i]
        t1, x1, y1 = sensor1_data[i + 1]

        for j in range(1, points_per_segment + 1):
            # Same linear interpolation as in position_interpolator.cpp

            alpha = j / (points_per_segment + 1)

            timestamp = t0 + alpha * (t1 - t0)

            x = x0 + alpha * (x1 - x0)
            y = y0 + alpha * (y1 - y0)

            interpolated_t.append(timestamp)
            interpolated_x.append(x)
            interpolated_y.append(y)

    fig = go.Figure()

    # Original trajectory, connecting real points
    fig.add_trace(go.Scatter3d(
        x=original_x,
        y=original_y,
        z=original_t,
        mode="lines+markers",
        name="Original sensor1 points",
        line=dict(color="royalblue", width=4),
        marker=dict(size=5, color="royalblue"),
        hovertemplate=(
            "Original point"
            "<br>Timestamp: %{z:.6f}"
            "<br>X: %{x:.6f}"
            "<br>Y: %{y:.6f}"
            "<extra></extra>"
        )
    ))

    # Interpolated positions
    fig.add_trace(go.Scatter3d(
        x=interpolated_x,
        y=interpolated_y,
        z=interpolated_t,
        mode="markers",
        name="Interpolated points",
        marker=dict(
            size=4,
            color="darkorange",
            symbol="diamond",
            opacity=0.9
        ),
        hovertemplate=(
            "Interpolated point"
            "<br>Timestamp: %{z:.6f}"
            "<br>X: %{x:.6f}"
            "<br>Y: %{y:.6f}"
            "<extra></extra>"
        )
    ))

    fig.update_layout(
        title="Sensor 1 — Linear Interpolation Visualization",
        scene=dict(
            xaxis_title="X",
            yaxis_title="Y",
            zaxis_title="Timestamp",
        ),
        width=1100,
        height=850,
        legend=dict(x=0.02, y=0.98),
    )

    fig.write_html("interpolation1.html")

def main():
    sensor1_data = parse_csv("sensor1.csv")
    sensor2_data = parse_csv("sensor2.csv")

    plotly_visualization(sensor1_data, "sensor1.html")
    plotly_visualization(sensor2_data, "sensor2.html")
    pos_delta = calc_pos_delta(sensor1_data, sensor2_data)

    interpolation_visualization1(sensor1_data)
    interpolation_visualization2(sensor2_data)

    plotly_visualization(pos_delta, "posdelta.html")


if __name__ == '__main__':
    main()

