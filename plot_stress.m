% plot_stress.m
% 万神殿穹顶薄膜应力分布 + 57°转变线
% 数据来源: C++ 输出的 stress_profile.csv
% MATLAB R2026a

clear; close all;

% ========== 读取 stress_profile.csv ==========
T = readtable('stress_profile.csv');
phi_deg     = T.phi_deg;
thickness_m = T.thickness_m;
density_kg  = T.density_kgm3;
N_phi_MN    = T.N_phi_MNpm;
N_theta_MN  = T.N_theta_MNpm;
sigma_theta = T.sigma_theta_MPa;

% ========== 57°转变线 ==========
phi_c_rad = acos((sqrt(5) - 1) / 2);
phi_c_deg = rad2deg(phi_c_rad);

fprintf('理论转变线 (从球顶起算): %.1f°\n', phi_c_deg);

% ========== 图1: 薄膜力分布 ==========
figure('Position', [50, 100, 1000, 420], 'Name', '薄膜力分布');

subplot(1,2,1);
hold on;
plot(phi_deg, N_phi_MN,   'b-',  'LineWidth', 2.2);
plot(phi_deg, N_theta_MN, 'r-',  'LineWidth', 2.2);
yline(0, 'k--', 'LineWidth', 0.8);
xline(phi_c_deg, 'k:', 'LineWidth', 1.3);

yl = ylim;
x_fill = [phi_c_deg, max(phi_deg), max(phi_deg), phi_c_deg];
y_fill = [yl(1), yl(1), yl(2), yl(2)];
patch(x_fill, y_fill, 'red', 'FaceAlpha', 0.06, 'EdgeColor', 'none');
text(mean([phi_c_deg, max(phi_deg)]), yl(2)*0.85, '环向受拉区', ...
    'Color', [0.7 0 0], 'FontSize', 11, 'HorizontalAlignment', 'center');

x_fill2 = [min(phi_deg), phi_c_deg, phi_c_deg, min(phi_deg)];
patch(x_fill2, y_fill, 'blue', 'FaceAlpha', 0.04, 'EdgeColor', 'none');
text(mean([min(phi_deg), phi_c_deg]), yl(2)*0.85, '环向受压区', ...
    'Color', [0 0 0.7], 'FontSize', 11, 'HorizontalAlignment', 'center');

hold off;
xlabel('纬度 $\phi$ (度，从球顶起算)', 'Interpreter', 'latex', 'FontSize', 13);
ylabel('薄膜力 (MN/m)', 'Interpreter', 'latex', 'FontSize', 13);
title('球壳薄膜力分布', 'FontSize', 14);
legend({'$N_\phi$ (经线力)', '$N_\theta$ (环向力)'}, ...
    'Interpreter', 'latex', 'FontSize', 11, 'Location', 'southwest');
grid on; box on;

% ========== 图1右: 环向应力 + f_t ==========
subplot(1,2,2);
hold on;
plot(phi_deg, sigma_theta, 'r-', 'LineWidth', 2.2);
yline(1.2, 'k--', 'LineWidth', 1.2);
yline(0,   'k-',  'LineWidth', 0.5);
xline(phi_c_deg, 'k:', 'LineWidth', 1.3);

text(max(phi_deg)-3, 1.35, '$f_t \approx 1.2$ MPa', ...
    'Interpreter', 'latex', 'FontSize', 11, 'Color', [0.3 0.3 0.3]);
text(phi_c_deg+1, 0.08, sprintf('%.0f°', phi_c_deg), ...
    'FontSize', 10, 'Color', [0.4 0.4 0.4]);

idx = sigma_theta > 1.2;
if any(idx)
    area(phi_deg(idx), sigma_theta(idx), 1.2, ...
        'FaceColor', 'red', 'FaceAlpha', 0.12, 'EdgeColor', 'none');
end

hold off;
xlabel('纬度 $\phi$ (度，从球顶起算)', 'Interpreter', 'latex', 'FontSize', 13);
ylabel('环向应力 $\sigma_\theta$ (MPa)', 'Interpreter', 'latex', 'FontSize', 13);
title('环向应力 vs 抗拉强度', 'FontSize', 14);
grid on; box on;

saveas(gcf, 'fig_stress_profile.png');
fprintf('已保存: fig_stress_profile.png\n');
