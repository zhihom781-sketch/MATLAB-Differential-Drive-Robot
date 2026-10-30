# Differential Drive Robot Motion and Control

A MATLAB, Simulink and Simscape-based simulation of differential drive robot motion, kinematic modelling, and trajectory deviations under simulated physical disturbances.

## Overview

Differential drive robots are widely used in mobile robotics due to their mechanical simplicity and maneuverability. By independently controlling the rotational velocities of their left and right wheels, these robots can perform various movements, including straight-line, circular, and curved trajectories.

This project develops a differential drive robot simulation using **MATLAB, Simulink and Simscape**. It implements the robot's kinematic equations, generates different motion patterns, and introduces simulated disturbances to investigate their effects on trajectory accuracy.

The primary objective is to compare ideal mathematical predictions against simulated robot behaviour under non-ideal operating conditions.

## System Parameters

| Parameter | Value |
|---|---:|
| Wheel Radius ($r$) | 0.05 m |
| Wheelbase ($L$) | 0.30 m |
| Simulation Duration | 10 s |
| Robot Configuration | Differential Drive |
| Kinematic Assumption | No wheel slip |

## System Architecture

The simulation consists of three main components.

### 1. Motion Control

Generates wheel velocity commands to produce three motion patterns:

- **Straight-Line Motion:** Both wheels rotate at equal angular velocities.
- **Circular Motion:** Constant but unequal wheel velocities generate a circular trajectory.
- **Curved Motion:** Varying wheel velocities produce a non-linear trajectory.

A Multiport Switch is used to select between the different motion patterns.

### 2. Disturbance Model

Uses Simscape components to simulate non-ideal physical effects, including motor friction, inertia, and electrical resistance.

These effects influence the simulated system's behaviour, allowing investigation of how deviations from ideal wheel velocities affect the robot's movement.

### 3. Differential Drive Kinematics

Calculates the robot's position and orientation based on the angular velocities of its two wheels.

**Linear Velocity**

$$
v = \frac{r}{2}(\omega_R+\omega_L)
$$

**Angular Velocity**

$$
\dot{\theta} = \frac{r}{L}(\omega_R-\omega_L)
$$

**Position Updates**

$$
\dot{x}=v\cos(\theta)
$$

$$
\dot{y}=v\sin(\theta)
$$

Where:

- $r$ = Wheel radius
- $L$ = Distance between the two wheels (wheelbase)
- $\omega_R$ = Right wheel angular velocity
- $\omega_L$ = Left wheel angular velocity
- $v$ = Linear velocity of the robot
- $\dot{\theta}$ = Angular velocity of the robot
- $x,y$ = Robot position in the global coordinate frame
- $\theta$ = Robot heading angle

The robot's position and orientation are obtained by integrating these velocity equations over time.

## Simulation Scenarios

### 1. Straight-Line Motion

Both wheels operate at equal angular velocities, theoretically producing a straight trajectory along the X-axis.

The simulation demonstrates relatively stable movement with minimal heading deviation under the tested conditions.

### 2. Circular Motion

Constant but unequal wheel angular velocities generate a circular trajectory.

The simulation investigates deviations between ideal and disturbed trajectories, including accumulated heading errors that affect positional accuracy.

### 3. Curved Motion

Varying wheel velocities generate a non-linear trajectory.

This scenario evaluates the robot's response to changing motion commands and disturbances, demonstrating how variations in wheel speeds can accumulate into positional and orientation errors.

## Results and Analysis

Simulation outputs are exported from Simulink into the MATLAB workspace for post-processing and visualization.

The analysis compares two trajectories:

- **Ideal Trajectory:** Reconstructed using differential drive kinematics and the simulated wheel angular velocities.
- **Actual Trajectory:** Obtained from the Simulink kinematic model, incorporating the effects of simulated disturbances.

MATLAB scripts generate the following visualizations:

1. XY trajectory comparison between ideal and disturbed motion.
2. Robot heading angle ($\theta$) over simulation time.

The results illustrate how differences between left and right wheel velocities introduce angular deviations. These deviations can accumulate over time, resulting in increasing positional errors.

The effects are particularly relevant during circular and curved motion, where changes in heading influence the robot's subsequent direction of travel.

## Key Findings

- Differential drive kinematics provide a mathematical foundation for modelling mobile robot movement.
- Small differences in wheel angular velocities can introduce heading deviations.
- Physical disturbances can reduce trajectory accuracy compared with ideal mathematical predictions.
- Open-loop motion control cannot actively correct accumulated trajectory errors.
- Feedback mechanisms can improve motion accuracy by compensating for disturbances and modelling uncertainties.

## Limitations and Future Improvements

The current implementation has several limitations.

- **Open-loop control:** The system does not implement closed-loop trajectory tracking to correct positional deviations.
- **No-slip assumption:** The kinematic model assumes that wheel rotation translates directly into linear displacement without slipping.
- **Disturbance modelling:** Simulated physical effects approximate real-world motor behaviour and may not capture every factor affecting an actual mobile robot.

Potential improvements include:

- Implementing wheel encoder feedback for more accurate wheel velocity estimation.
- Introducing PID controllers for wheel velocity regulation and trajectory tracking.
- Investigating the effects of wheel slip and external disturbances.
- Evaluating tracking accuracy using quantitative metrics such as root mean square error (RMSE).
- Extending the simulation to more complex trajectories and navigation scenarios.

## Technologies Used

- **MATLAB** – Mathematical modelling, numerical integration, and simulation analysis.
- **Simulink** – System modelling, motion control, and kinematic implementation.
- **Simscape** – Physical system modelling and disturbance simulation.

## Repository Contents

| File / Folder | Description |
|---|---|
| `*.slx` | Simulink models for robot motion, disturbance modelling, and kinematics |
| `*.m` | MATLAB scripts for simulation analysis and trajectory visualization |
| `README.md` | Project documentation |
