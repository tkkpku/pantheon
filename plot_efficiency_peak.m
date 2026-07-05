% plot_efficiency_peak.m
% 效率函数 E(f) = k·f·(1-αf) 的理论分析
% 回答: 效率峰值在哪？罗马人为什么没到？

clear; close all;

% E层参数
d0_E = 0.48;   % 基准深度 (m)
t_E  = 2.87;   % E层穹顶厚度 (m)
alpha = d0_E / t_E;  % ≈ 0.167

% 效率函数（归一化）: E ∝ f × (1 - αf)
f_cont = linspace(0, 5.5, 200);
E_norm = f_cont .* (1 - alpha * f_cont);
E_norm(E_norm < 0) = NaN;

f_star = 1 / (2 * alpha);  % 理论峰值
E_star = f_star * (1 - alpha * f_star);

% 实测数据
factor_meas = [0.60, 0.80, 1.00, 1.20, 1.40];
E_meas = [98.3, 127.0, 153.5, 177.8, 199.9];

figure('Position', [100, 100, 820, 500], 'Name', '效率峰值分析');
hold on;

% 理论曲线
plot(f_cont, E_norm / max(E_norm) * max(E_meas), 'b-', 'LineWidth', 2);
% 实测点
plot(factor_meas, E_meas, 'ro', 'MarkerSize', 12, 'MarkerFaceColor', 'r', ...
    'LineWidth', 1.5);
% 峰值
plot(f_star, E_star / max(E_norm) * max(E_meas), 'm^', ...
    'MarkerSize', 18, 'MarkerFaceColor', 'm', 'LineWidth', 1.5);
% 实际设计
xline(1.0, 'k--', 'LineWidth', 1.8);

% 标注
text(f_star + 0.15, E_star / max(E_norm) * max(E_meas) + 10, ...
    sprintf('理论峰值\nf* = %.1f (深度 %.2f m)', f_star, d0_E * f_star), ...
    'FontSize', 11, 'FontWeight', 'bold', 'Color', 'm');
text(1.05, 70, '实际设计\nf = 1.0', 'FontSize', 11, 'Color', [0.3 0.3 0.3]);

% 不可达区域
x_patch = [1.8, 5.5, 5.5, 1.8];
y_patch = [0, 0, max(E_meas)*1.1, max(E_meas)*1.1];
patch(x_patch, y_patch, [0.85 0.85 0.85], 'FaceAlpha', 0.35, ...
    'EdgeColor', 'none');
text(3.5, max(E_meas)*0.5, '不可达区域\n(藻井深度 > 弧高\n或挖穿穹顶)', ...
    'FontSize', 11, 'FontWeight', 'bold', 'HorizontalAlignment', 'center', ...
    'Color', [0.4 0.4 0.4]);

hold off;
xlabel('深度因子 $f$', 'Interpreter', 'latex', 'FontSize', 14);
ylabel('综合效率 $E$', 'Interpreter', 'latex', 'FontSize', 14);
title('效率函数 $E(f) \\propto f(1-\\alpha f)$: 理论峰值在不可达区域', ...
    'Interpreter', 'latex', 'FontSize', 14);
legend({'理论 $E(f)$', '实测数据', ...
    sprintf('理论峰值 $f^*$=%.1f', f_star), '实际设计'}, ...
    'FontSize', 11, 'Location', 'northwest');
grid on; box on;
xlim([0, 5.5]);

saveas(gcf, 'fig_efficiency_peak.png');
fprintf('已保存: fig_efficiency_peak.png\n');
fprintf('\n理论峰值: f* = %.1f (深度 %.2f m)\n', f_star, d0_E * f_star);
fprintf('此时藻井深度/弧高 = %.2f / %.2f = %.2f\n', ...
    d0_E * f_star, 2.39, d0_E * f_star / 2.39);
fprintf('结论: 峰值在物理上不可达（深度 > 藻井弧高的一半）\n');
