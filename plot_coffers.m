% plot_coffers.m (v3 - no Chinese identifiers)
% Coffer geometry + mass reduction + arch model key numbers
% MATLAB R2026a

clear; close all;

T = readtable('coffer_details.csv');
% Columns: 1=layer, 2=latitude, 3=arc_height, 4=w_bottom,
%          5=w_top, 6=depth, 7=count, 8=V_single, 9=V_layer, 10=mass

layer_names = string(T{:,1});
arc_h  = T{:,3};
w_bot  = T{:,4};
w_top  = T{:,5};
depth  = T{:,6};
V_lay  = T{:,9};
M_lay  = T{:,10};

step_depth = [0.23 0.19 0.13 0.12;
              0.22 0.18 0.13 0.12;
              0.21 0.18 0.12 0.10;
              0.20 0.16 0.11 0.07;
              0.19 0.15 0.14  NaN];

M_total  = 24587.5;
M_coffer = 1987.8;
B_pct    = 8.085;

figure('Position', [30, 60, 1150, 420]);

subplot(1,5,1); x = 1:5; hold on;
plot(x, w_bot, 'bo-', 'LineWidth', 2, 'MarkerFaceColor', 'b', 'MarkerSize', 7);
plot(x, w_top, 'rs-', 'LineWidth', 2, 'MarkerFaceColor', 'r', 'MarkerSize', 7);
plot(x, arc_h, 'g^-', 'LineWidth', 2, 'MarkerFaceColor', 'g', 'MarkerSize', 7);
plot(x, depth, 'md-', 'LineWidth', 2, 'MarkerFaceColor', 'm', 'MarkerSize', 7);
hold off;
set(gca, 'XTick', 1:5, 'XTickLabel', layer_names);
xlabel('Layer'); ylabel('m');
title('Coffer Dimensions'); legend({'BotW','TopW','ArcH','Depth'}, 'FontSize', 7);
grid on; box on;

subplot(1,5,2);
bar(step_depth, 'stacked', 'BarWidth', 0.7);
set(gca, 'XTick', 1:5, 'XTickLabel', layer_names);
xlabel('Layer'); ylabel('m');
title('4-Step Depth');
grid on; box on;

subplot(1,5,3);
bar_colors = lines(5);
h_bar = bar(M_lay, 'FaceColor', 'flat');
for i = 1:5; h_bar.CData(i,:) = bar_colors(i,:); end
set(gca, 'XTick', 1:5, 'XTickLabel', layer_names);
xlabel('Layer'); ylabel('tonnes');
title('Mass Removed');
for i = 1:5
    text(i, M_lay(i)+12, sprintf('%.0f', M_lay(i)), ...
        'HorizontalAlignment', 'center', 'FontSize', 7, 'FontWeight', 'bold');
end
grid on; box on;

subplot(1,5,4);
pie([M_coffer, M_total-M_coffer], ...
    {sprintf('Coffers\n%.0f t', M_coffer), sprintf('Dome\n%.0f t', M_total-M_coffer)});
title(sprintf('Reduction %.1f%%', B_pct), 'FontSize', 12);

subplot(1,5,5);
axis off;
text(0, 0.9, 'Arch Model', 'FontSize', 14, 'FontWeight', 'bold');
text(0, 0.7, '28 wedge arches', 'FontSize', 12);
text(0, 0.55, 'Self-weight/arch:', 'FontSize', 11);
text(0, 0.45, sprintf('8.6 -> 7.9 MN (-%.1f%%)', B_pct), 'FontSize', 12, 'FontWeight', 'bold');
text(0, 0.3, 'Thrust/arch:', 'FontSize', 11);
text(0, 0.2, sprintf('3.5 -> 3.2 MN (-%.1f%%)', B_pct), 'FontSize', 12, 'FontWeight', 'bold');
text(0, 0.05, sprintf('JCIR = %.1f%%', B_pct), 'FontSize', 12, 'Color', [0 0.6 0], 'FontWeight', 'bold');

saveas(gcf, 'figs/fig02_coffer_data.png');
fprintf('Saved: figs/fig02_coffer_data.png\n');
