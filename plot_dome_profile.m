% plot_dome_profile.m (v3)
% Dome cross-section with arch model + two crack types
% MATLAB R2026a

clear; close all;

R = 22.03;  r_oc = 4.45;
phi_oc = asin(r_oc / R);  phi_max = pi/2;
t_oc = 1.2;  t_base = 5.9;

phi_arr = linspace(phi_oc, phi_max, 300);
x_inner = R * sin(phi_arr);
y_inner = R * cos(phi_arr);
for i = 1:length(phi_arr)
    phi = phi_arr(i);
    ratio = (phi - phi_oc) / (phi_max - phi_oc);
    t = t_oc + (t_base - t_oc) * ratio;
    x_outer(i) = (R + t) * sin(phi);
    y_outer(i) = (R + t) * cos(phi);
end

figure('Position', [40, 40, 780, 720]);
hold on;

fill([x_inner, fliplr(x_outer)], [-y_inner, -fliplr(y_outer)], ...
     [0.82 0.78 0.68], 'EdgeColor', 'none');

phi_crack_start = (90 - 57) * pi / 180;
phi_crack_end   = pi/2;
idx = phi_arr >= phi_crack_start & phi_arr <= phi_crack_end;
x_cr = x_inner(idx);  y_cr = -y_inner(idx);
for j = 1:8:length(x_cr)
    plot([x_cr(j), x_cr(j) + 1.3*sign(x_cr(j))], [y_cr(j), y_cr(j)], ...
         'r-', 'LineWidth', 1.2);
end

plot(x_inner, -y_inner, 'k-', 'LineWidth', 2.5);
plot(x_outer, -y_outer, 'k-', 'LineWidth', 1.5);

y_oc = sqrt(R^2 - r_oc^2);
plot([-r_oc, -r_oc], [-y_oc, -y_oc-t_oc], 'k-', 'LineWidth', 2);
plot([ r_oc,  r_oc], [-y_oc, -y_oc-t_oc], 'k-', 'LineWidth', 2);
text(0, -y_oc - t_oc - 4, sprintf('Oculus  o%.1f m', 2*r_oc), ...
    'FontSize', 11, 'HorizontalAlignment', 'center', 'FontWeight', 'bold');

xd = 26;
x_drum = [x_outer(end), x_outer(end), xd, xd];
yt = -y_outer(end);
y_drum = [yt, yt-22, yt-22, yt];
fill(x_drum, y_drum, [0.78 0.73 0.63], 'EdgeColor', 'k', 'LineWidth', 1.5);
fill(-x_drum, y_drum, [0.78 0.73 0.63], 'EdgeColor', 'k', 'LineWidth', 1.5);
text(xd+1.5, mean(y_drum(1:2)), 'Drum 6m', 'FontSize', 10, 'FontWeight', 'bold');

h = [4.08, 3.79, 3.52, 3.05, 2.39];
band = 0.84;
labels = {'A','B','C','D','E'};
colors = lines(5);
cum_arc = band / 2;
for i = 1:5
    phi_low  = phi_max - cum_arc / R;
    phi_high = phi_max - (cum_arc + h(i)) / R;
    phi_mid  = (phi_low + phi_high) / 2;
    phi_plt  = linspace(phi_high, phi_low, 40);
    plot(R*sin(phi_plt), -R*cos(phi_plt), '-', 'Color', colors(i,:), 'LineWidth', 5);
    text(R*sin(phi_mid)+2.5, -R*cos(phi_mid), labels{i}, ...
        'Color', colors(i,:), 'FontSize', 11, 'FontWeight', 'bold');
    cum_arc = cum_arc + h(i) + band;
end

phi_57 = (90 - 57) * pi / 180;
x57 = R * sin(phi_57);  y57 = -R * cos(phi_57);
plot([-x57, x57], [y57, y57], 'r--', 'LineWidth', 1.5);
text(0, y57 + 2.5, 'Meridional crack stop (~57deg)', 'FontSize', 10, ...
    'Color', 'red', 'HorizontalAlignment', 'center', 'FontWeight', 'bold');
plot(x57, y57, 'ro', 'MarkerSize', 8, 'MarkerFaceColor', 'r');

plot([x_outer(end), xd], [yt-1, yt-1], 'm-', 'LineWidth', 3);
text((x_outer(end)+xd)/2 + 1, yt-3, 'Junction crack', ...
    'FontSize', 10, 'Color', 'm', 'FontWeight', 'bold');

quiver(x_outer(end), yt, -3, 2, 'b-', 'LineWidth', 2.5, 'MaxHeadSize', 2);
text(x_outer(end)-5, yt+4, 'Thrust H', 'FontSize', 10, ...
    'Color', 'b', 'FontWeight', 'bold');

phi_arch = linspace(phi_oc, phi_max, 80);
x_arch = R*0.15 * sin(phi_arch);
y_arch = -R * cos(phi_arch);
plot(x_arch, y_arch, 'b-', 'LineWidth', 2);
text(5, -8, 'Wedge arch', 'FontSize', 11, 'Color', 'b', 'FontWeight', 'bold');

plot(0, 0, 'k+', 'MarkerSize', 14, 'LineWidth', 2.5);
text(1.5, 1.5, 'Center O', 'FontSize', 11, 'FontWeight', 'bold');

plot([-R, R], [-56, -56], 'k-', 'LineWidth', 1);
plot([-R, -R], [-57, -55], 'k-', 'LineWidth', 1);
plot([R, R], [-57, -55], 'k-', 'LineWidth', 1);
text(0, -59, sprintf('D = %.1f m', 2*R), ...
    'FontSize', 11, 'HorizontalAlignment', 'center', 'FontWeight', 'bold');

axis equal;
xlim([-30, 32]);  ylim([-63, 12]);
xlabel('x (m)', 'FontSize', 13);
ylabel('y (m)', 'FontSize', 13);
title('Pantheon Dome - Cracked Arch Model & Two Crack Types', 'FontSize', 15);
grid on; box on;

text(18, -30, '| Meridional cracks', 'FontSize', 10, 'Color', 'red');
text(18, -33, '--- 57deg stop line', 'FontSize', 10, 'Color', 'red');
text(18, -36, '- Junction crack', 'FontSize', 10, 'Color', 'm');
text(18, -39, '-> Arch thrust H', 'FontSize', 10, 'Color', 'b');

saveas(gcf, 'figs/fig05_dome_profile.png');
fprintf('Saved: figs/fig05_dome_profile.png\n');
