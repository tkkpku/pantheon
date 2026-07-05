% plot_dome_profile.m
% 万神殿穹顶子午线剖面图，标注藻井位置
% MATLAB R2026a

clear; close all;

R = 22.03;  r_oc = 4.45;
phi_oc = asin(r_oc / R);  phi_max = pi/2;
t_oc = 1.2;  t_base = 5.9;

% ========== 穹顶轮廓 ==========
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

figure('Position', [50, 50, 720, 680], 'Name', '穹顶剖面');
hold on;

% 混凝土填充
fill([x_inner, fliplr(x_outer)], [-y_inner, -fliplr(y_outer)], ...
    [0.82 0.78 0.68], 'EdgeColor', 'none');

% 内外轮廓
plot(x_inner, -y_inner, 'k-', 'LineWidth', 2.5);
plot(x_outer, -y_outer, 'k-', 'LineWidth', 1.5);

% Oculus
y_oc = sqrt(R^2 - r_oc^2);
plot([-r_oc, -r_oc], [-y_oc, -y_oc - t_oc], 'k-', 'LineWidth', 2);
plot([ r_oc,  r_oc], [-y_oc, -y_oc - t_oc], 'k-', 'LineWidth', 2);
fill([-r_oc, r_oc, r_oc, -r_oc], [-y_oc, -y_oc, -y_oc-t_oc, -y_oc-t_oc], ...
    [0.75 0.7 0.6], 'EdgeColor', 'k', 'LineWidth', 1);

text(0, -y_oc - t_oc - 4, sprintf('Oculus  ø%.1f m', 2*r_oc), ...
    'FontSize', 11, 'HorizontalAlignment', 'center', 'FontWeight', 'bold');

% 鼓座
x_drum = [x_outer(end), x_outer(end), 26, 26];
y_drum = [-y_outer(end), -y_outer(end)-22, -y_outer(end)-22, -y_outer(end)];
fill(x_drum, y_drum, [0.78 0.73 0.63], 'EdgeColor', 'k', 'LineWidth', 1.5);
fill(-x_drum, y_drum, [0.78 0.73 0.63], 'EdgeColor', 'k', 'LineWidth', 1.5);
text(27, mean(y_drum(1:2)), '鼓座 6m', 'FontSize', 10, 'FontWeight', 'bold');

% ========== 标注藻井 ==========
h = [4.08, 3.79, 3.52, 3.05, 2.39];
band = 0.84;
layer_labels = {'A', 'B', 'C', 'D', 'E'};
colors = lines(5);

cum_arc = band / 2;
for i = 1:5
    phi_low  = phi_max - cum_arc / R;
    phi_high = phi_max - (cum_arc + h(i)) / R;
    phi_mid  = (phi_low + phi_high) / 2;

    phi_plot = linspace(phi_high, phi_low, 40);
    x_plot = R * sin(phi_plot);
    y_plot = -R * cos(phi_plot);
    plot(x_plot, y_plot, '-', 'Color', colors(i,:), 'LineWidth', 5);

    text(R*sin(phi_mid) + 2.2, -R*cos(phi_mid), layer_labels{i}, ...
        'Color', colors(i,:), 'FontSize', 11, 'FontWeight', 'bold');

    cum_arc = cum_arc + h(i) + band;
end

% 球心
plot(0, 0, 'k+', 'MarkerSize', 14, 'LineWidth', 2.5);
text(1.2, 1.0, '球心 O', 'FontSize', 11, 'FontWeight', 'bold');

% 跨度标注
plot([-R, R], [-52, -52], 'k-', 'LineWidth', 1);
plot([-R, -R], [-53, -51], 'k-', 'LineWidth', 1);
plot([R, R], [-53, -51], 'k-', 'LineWidth', 1);
text(0, -55, sprintf('内径 D = %.1f m (激光扫描实测)', 2*R), ...
    'FontSize', 11, 'HorizontalAlignment', 'center', 'FontWeight', 'bold');

axis equal;
xlim([-30, 32]);
ylim([-60, 10]);
xlabel('x (m)', 'FontSize', 13);
ylabel('y (m)', 'FontSize', 13);
title('万神殿穹顶子午线剖面 —— 藻井位置 (A–E)', 'FontSize', 15);
grid on; box on;

saveas(gcf, 'fig_dome_profile.png');
fprintf('已保存: fig_dome_profile.png\n');
