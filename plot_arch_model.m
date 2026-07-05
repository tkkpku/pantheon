% plot_arch_model.m
% Wedge arch visualization + thrust + JCF analysis
% MATLAB R2026a

clear; close all;

B_pct    = 8.085;
thrust_0 = 3.5;
thrust_B = 3.2;
n_arches = 28;

figure('Position', [80, 80, 900, 550]);

subplot(2,3,[1 2 4 5]);
hold on;
R_plot = 22;
theta = linspace(0, 2*pi, n_arches+1);
phi_drum = linspace(0, 2*pi, 100);
fill(R_plot*1.35*cos(phi_drum), R_plot*1.35*sin(phi_drum), ...
    [0.85 0.8 0.7], 'EdgeColor', 'k', 'LineWidth', 2);
for i = 1:n_arches
    t1 = theta(i); t2 = theta(i+1);
    patch([0, R_plot*cos(t1), R_plot*cos(t2), 0], ...
          [0, R_plot*sin(t1), R_plot*sin(t2), 0], ...
          [0.9 0.85 0.75], 'EdgeColor', [0.5 0.4 0.3]);
end
t1_h = theta(7); t2_h = theta(8);
patch([0, R_plot*cos(t1_h), R_plot*cos(t2_h), 0], ...
      [0, R_plot*sin(t1_h), R_plot*sin(t2_h), 0], ...
      [1 0.8 0.5], 'EdgeColor', 'r', 'LineWidth', 2.5);
t_mid = (t1_h + t2_h)/2;
text(R_plot*0.55*cos(t_mid), R_plot*0.55*sin(t_mid), ...
    '1 wedge arch', 'FontSize', 10, 'FontWeight', 'bold', 'Color', 'r', ...
    'HorizontalAlignment', 'center');
text(0, 0, 'Oculus', 'FontSize', 11, 'HorizontalAlignment', 'center', 'FontWeight', 'bold');
text(R_plot*1.4, 0, 'Drum', 'FontSize', 11, 'FontWeight', 'bold', 'HorizontalAlignment', 'center');
axis equal;
xlim([-R_plot*1.6, R_plot*1.6]);
ylim([-R_plot*1.6, R_plot*1.6]);
title(sprintf('Cracked Dome = %d Independent Wedge Arches', n_arches), 'FontSize', 14);
axis off;

subplot(2,3,3);
cats = {'No Coffers', sprintf('With Coffers (B=%.1f%%)', B_pct)};
tv = [thrust_0, thrust_B];
h_bar = bar(tv, 0.5, 'FaceColor', [0.3 0.5 0.8]);
set(gca, 'XTickLabel', cats, 'FontSize', 9);
ylabel('Thrust per Arch (MN)', 'FontSize', 12);
title('Arch Horizontal Thrust', 'FontSize', 13);
for i = 1:2
    text(i, tv(i) + 0.08, sprintf('%.1f MN', tv(i)), ...
        'HorizontalAlignment', 'center', 'FontSize', 11, 'FontWeight', 'bold');
end
grid on; box on;

subplot(2,3,6);
jv = [0.070, 0.064];
h_bar2 = bar(jv, 0.5, 'FaceColor', [0.8 0.3 0.3]);
set(gca, 'XTickLabel', cats, 'FontSize', 9);
ylabel('JCF', 'FontSize', 12);
title('Junction Crack Factor', 'FontSize', 13);
for i = 1:2
    text(i, jv(i) + 0.0015, sprintf('%.3f', jv(i)), ...
        'HorizontalAlignment', 'center', 'FontSize', 11, 'FontWeight', 'bold');
end
text(1.5, max(jv)*0.6, sprintf('JCIR = %.1f%%', B_pct), ...
    'FontSize', 12, 'FontWeight', 'bold', 'Color', [0 0.6 0], ...
    'HorizontalAlignment', 'center');
grid on; box on;

saveas(gcf, 'fig_arch_model.png');
fprintf('Saved: fig_arch_model.png\n');
