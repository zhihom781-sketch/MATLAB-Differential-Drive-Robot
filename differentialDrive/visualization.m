%% This part is for testing
t = out.tout;

wl = out.omega_left_out;
wr = out.omega_right_out;
r = out.wheel_radius_out;
L = out.wheel_base_out;

v = (r/2) .* (wr + wl);
omega = (wr - wl) ./ L;

theta = cumtrapz(t, omega);

x_dot_ideal = v .* cos(theta);
y_dot_ideal = v .* sin(theta);

x_ideal = cumtrapz(t, x_dot_ideal);
y_ideal = cumtrapz(t, y_dot_ideal);

% % Extracting data (This is the actual one)
time  = out.tout;        % The time vector
x_pos = out.x_out;       % 101x1 matrix of X coordinates
y_pos = out.y_out;       % 101x1 matrix of Y coordinates
theta = out.theta_out;   % 101x1 matrix of orientation

% Plotting
figure(1);
plot(x_ideal, y_ideal, 'y--', 'LineWidth', 1.2, 'DisplayName', 'Predicted (Ideal)');
hold on;
plot(x_pos, y_pos, 'r-', 'LineWidth', 1.5, 'DisplayName', 'Actual (Disturbed)');
grid on;
axis equal; % Ensures the geometry is accurate
xlabel('X Position (m)');
ylabel('Y Position (m)');
title('Trajectory Comparison: Ideal vs. Disturbed Path');
legend('Location', 'best');

figure(2);
plot(time, theta, 'b-', 'LineWidth', 1.5);
grid on;
xlabel('Time (s)');
ylabel('Heading Angle (rad)');
title('Robot Orientation (Heading) Over Time');