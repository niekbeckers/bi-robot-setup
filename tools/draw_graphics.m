clc
close all

% loading this mat file takes a looong time! SO comment it by default
% load('ALLresults.mat')

n_exp = [2:16];
d_experiments = [4:15];
s_experiments = [2 3 16]; % ATTENTION! e16 was done with only 1 subject sitting at robot1
g1_experiments = [7 8 12 13]; % group 1: connection stiffness 50 N/m
g2_experiments = [4 10 11 15]; % group 2: connection stiffness 125 N/m
g3_experiments = [5 6 9 14]; % group 3: connection stiffness 250 N/m

trials = 1:48;
trials = trials';
%% PLOT  IMPROVEMENT DUAL TRIAL VS RELATIVE PERFORMANCE
f = figure()
set(f, 'Color', 'w');

g1_relative_performance = [];
g1_improv_dual = [];
for ii = g1_experiments
    
    d1 = plot(experiments_struct.Experiment{1,ii}.relative_perf_r1*100, experiments_struct.Experiment{1, ii}.improv_dual_r1*100, 'ko', experiments_struct.Experiment{1,ii}.relative_perf_r2*100, experiments_struct.Experiment{1, ii}.improv_dual_r2*100, 'ko')
    hold on
    g1_relative_performance = [g1_relative_performance experiments_struct.Experiment{1,ii}.relative_perf_r1*100 experiments_struct.Experiment{1,ii}.relative_perf_r2*100];
    g1_improv_dual = [g1_improv_dual experiments_struct.Experiment{1, ii}.improv_dual_r1*100 experiments_struct.Experiment{1, ii}.improv_dual_r2*100];
end

p1 = polyfit(g1_relative_performance, g1_improv_dual,1);
x1 = linspace (-0.01*100,0.01*100, 1000);
y1 = polyval(p1,x1);
p1 = plot(x1,y1,'k--', 'LineWidth', 2);
hold on



g2_relative_performance = [];
g2_improv_dual = [];
for ii = g2_experiments
    
    d2 = plot(experiments_struct.Experiment{1,ii}.relative_perf_r1*100, experiments_struct.Experiment{1, ii}.improv_dual_r1*100, 'ro', experiments_struct.Experiment{1,ii}.relative_perf_r2*100, experiments_struct.Experiment{1, ii}.improv_dual_r2*100, 'ro')
    hold on
    
    g2_relative_performance = [g2_relative_performance experiments_struct.Experiment{1,ii}.relative_perf_r1*100 experiments_struct.Experiment{1,ii}.relative_perf_r2*100];
    g2_improv_dual = [g2_improv_dual experiments_struct.Experiment{1, ii}.improv_dual_r1*100 experiments_struct.Experiment{1, ii}.improv_dual_r2*100];
end
p2 = polyfit(g2_relative_performance, g2_improv_dual,1);
y2 = polyval(p2,x1);
p2 = plot(x1,y2,'r--', 'LineWidth', 2);
hold on


g3_relative_performance = [];
g3_improv_dual = [];
for ii = g3_experiments
    
    d3 = plot(experiments_struct.Experiment{1,ii}.relative_perf_r1*100, experiments_struct.Experiment{1, ii}.improv_dual_r1*100, 'bo', experiments_struct.Experiment{1,ii}.relative_perf_r2*100, experiments_struct.Experiment{1, ii}.improv_dual_r2*100, 'bo')
    hold on
    
    g3_relative_performance = [g3_relative_performance experiments_struct.Experiment{1,ii}.relative_perf_r1*100 experiments_struct.Experiment{1,ii}.relative_perf_r2*100];
    g3_improv_dual = [g3_improv_dual experiments_struct.Experiment{1, ii}.improv_dual_r1*100 experiments_struct.Experiment{1, ii}.improv_dual_r2*100];
end
p3 = polyfit(g3_relative_performance, g3_improv_dual,1);
y3 = polyval(p3,x1);
p3 = plot(x1,y3,'b--', 'LineWidth', 2);
hold on

title('Improvement during Interaction', 'FontSize', 20)
xlabel('Relative Performance of Partner in Single Trial [cm]', 'FontSize', 20)
ylabel('Improvement in Dual Trial [cm]', 'FontSize', 20)
legend([p1 p2 p3], 'Group 1', 'Group 2', 'Group 3')
xlim([-1 1])
grid on

hold off


all_relative_perf = [g1_relative_performance g2_relative_performance g3_relative_performance];
all_improv_dual = [g1_improv_dual g2_improv_dual g3_improv_dual];

% % fit to polynomial curve
% p1 = polyfit(all_relative_perf,all_improv_dual,1);
% x1 = linspace (-0.01*100,0.01*100, 1000);
% y1 = polyval(p1,x1);
% figure()
% p1 = plot(x1,y1,'b--', 'LineWidth', 2);
% hold on
% plot(all_relative_perf,all_improv_dual, 'ko');
% grid on
%
% title('Improvement during Interaction', 'FontSize', 20)
% xlabel('Relative Performance of Partner in Single Trial [cm]', 'FontSize', 20)
% ylabel('Improvement in Dual Trial [cm]', 'FontSize', 20)
% legend([p1], 'Polynomial 1st order fit')
% savefig('Improvement_during_interaction')
% hold off
%% PLOT IMPROVEMENT SINGLE TRIAL VS TRIALS

% all_improv_single = [];
% for ii = n_exp
%     all_improv_single = [all_improv_single; experiments_struct.Experiment{1, ii}.improv_single_r1; experiments_struct.Experiment{1, ii}.improv_single_r2];
% end
%
% figure()
% plot(trials(1:end-1), all_improv_single*100, 'o')
% title('Learning during Interaction', 'FontSize', 20)
% xlabel('Trials', 'FontSize', 20)
% ylabel('Improvement in Single Trial [cm]', 'FontSize', 20)
% savefig('Learning_during_interaction')
% hold off

%% PLOT IMPROVEMENT SINGLE TRIAL VS TRIALS DIVIDED BY GROUP

trials_g12 = [2 4 6 7 10 12 14 16 18 20 23 25 26 28 30 32 34 36 39 40 42 44 45];

% solos
improvs_mean_s = [];

for ii = s_experiments
    if ii == 16
        improvs_mean_s = mean([improvs_mean_s; experiments_struct.Experiment{1, ii}.improv_single_r1], 'omitnan');
        
    else
        improvs_mean_s = mean([improvs_mean_s; experiments_struct.Experiment{1, ii}.improv_single_r1; experiments_struct.Experiment{1, ii}.improv_single_r2], 'omitnan');
    end
    
end

improvs_s_fit = fit(trials(1:end-1), improvs_mean_s', 'poly1');

% GROUP 1: CONNECTION STIFFNESS = 50 N/m
improvs_mean_g1 = [];
for ii = g1_experiments
    improvs_mean_g1 = mean([improvs_mean_g1; experiments_struct.Experiment{1, ii}.improv_single_r1; experiments_struct.Experiment{1, ii}.improv_single_r2], 'omitnan');
    
end

improvs_g1_fit = fit(trials_g12', improvs_mean_g1(trials_g12)', 'poly1');

% GROUP 2: CONNECTION STIFFNESS = 125 N/m

improvs_mean_g2 = [];
for ii = g2_experiments
    improvs_mean_g2 = mean([improvs_mean_g2; experiments_struct.Experiment{1, ii}.improv_single_r1; experiments_struct.Experiment{1, ii}.improv_single_r2], 'omitnan');
    
end

improvs_g2_fit = fit(trials_g12', improvs_mean_g2(trials_g12)', 'poly1');

% GROUP 3: CONNECTION STIFFNESS = 250 N/m

improvs_mean_g3 = [];
for ii = g3_experiments
    
    improvs_mean_g3 = mean([improvs_mean_g3; experiments_struct.Experiment{1, ii}.improv_single_r1; experiments_struct.Experiment{1, ii}.improv_single_r2], 'omitnan');
    
end

improvs_g3_fit = fit(trials(1:end-2), improvs_mean_g3', 'poly1');


f = figure()

plot(improvs_s_fit, 'k');

hold on
plot(improvs_g1_fit, 'b');

hold on
plot(improvs_g2_fit, 'g')

hold on
plot(improvs_g3_fit, 'r')

title('Learning in Single Trials', 'FontSize', 20)
xlabel('Trials', 'FontSize', 20)
ylabel('Improvement in Single Trial[m]', 'FontSize', 20)
legend('Solo', 'Group 1', 'Group 2', 'Group 3')

xlim([1 47])
set(f, 'Color', 'w');
grid on

savefig('Learning Curves Groups')

%% PLOT ERRORS VS PHASE SETS BOXPLOT

all_errors = [];
all_phase_sequencies = [];

for ii = n_exp
    all_errors = [all_errors experiments_struct.Experiment{1, ii}.e_r1 experiments_struct.Experiment{1, ii}.e_r2];
    all_phase_sequencies = [all_phase_sequencies experiments_struct.Experiment{1, ii}.phases_sequence'  experiments_struct.Experiment{1, ii}.phases_sequence'];
end

f = figure()
set(f, 'Color', 'w');
boxplot(all_errors*100, all_phase_sequencies)
title('Mean Error in Trial by Phase sets', 'FontSize', 20)
xlabel('Phase Set', 'FontSize', 20)
ylabel('Mean Error in Trial [cm]', 'FontSize', 20)

savefig('Mean Error in Trial by Phase sets')
hold off
% % stat analysis
%
% [pv_EvsP, tb_EvsP, stats_EvsP] = anova1(all_errors, all_phase_sequencies);
% [results_EvsP,means_EvsP] = multcompare(stats_EvsP,'CType','bonferroni');

%% PLOT ALL ERRORS VS TRIALS

% dyads
all_errors_d = [];

for ii = d_experiments
    
    all_errors_d = [all_errors_d experiments_struct.Experiment{1, ii}.e_r1' experiments_struct.Experiment{1, ii}.e_r2'];
    
end

meanxtriald = mean(all_errors_d');
fdp1 = fit(trials, meanxtriald', 'exp1');
f = figure()
set(f, 'Color', 'w');
p2 = plot(fdp1, trials, all_errors_d);
hold on

% solos
all_errors_s = [];

for ii = s_experiments
    if ii == not(16)
        all_errors_s = [all_errors_s experiments_struct.Experiment{1, ii}.e_r1' experiments_struct.Experiment{1, ii}.e_r2'];
    else
        all_errors_s = [all_errors_s experiments_struct.Experiment{1, ii}.e_r1'];
    end
end

meanxtrials = mean(all_errors_s');
solofit = fit(trials, meanxtrials', 'exp1', 'Exclude', meanxtrials>0.01);
p3 = plot(solofit, trials, all_errors_s);
title('Learning curve', 'FontSize', 20)
xlabel('Trials', 'FontSize', 20)
ylabel('Mean Error during Trial [cm]', 'FontSize', 20)
savefig('Learning curve')
hold off

%% PLOT ALL ERRORS VS TRIALS DIVIDED BY GROUP

% SOLOS
f = figure()
set(f, 'Color', 'w');
p3 = plot(solofit, trials, all_errors_s);
title('Learning Curve Solo Group', 'FontSize', 20)
xlabel('Trials', 'FontSize', 20)
ylabel('Mean Error during Trial [cm]', 'FontSize', 20)
xlim([1 48])
savefig('Learning Curve Solo Group')
hold off
% GROUP 1: CONNECTION STIFFNESS = 50 N/m
all_errors_g1 = [];
err_mean_g1 = [];
f = figure()
set(f, 'Color', 'w');
ccount = 1;
for ii = g1_experiments
    switch ccount
        case 1
            cstring = 'k';
            cpstring = 'k--';
        case 2
            cstring = 'b';
            cpstring = 'b--';
        case 3
            cstring = 'r';
            cpstring = 'r--';
        case 4
            cstring = 'g';
            cpstring = 'g--';
    end
    all_errors_g1 = [all_errors_g1 experiments_struct.Experiment{1, ii}.e_r1' experiments_struct.Experiment{1, ii}.e_r2'];
    plot(experiments_struct.Experiment{1, ii}.expfit_r1, cstring)
    hold on
    plot(experiments_struct.Experiment{1, ii}.expfit_r2, cstring)
    hold on
    %     plot(trials, experiments_struct.Experiment{1, ii}.e_r1', cpstring, trials, experiments_struct.Experiment{1, ii}.e_r2', cpstring)
    %     hold on
    ccount = ccount+1;
    
    % calculate mean to have 1 curve per dyad
    err_mean_temp = mean([experiments_struct.Experiment{1, ii}.e_r1; experiments_struct.Experiment{1, ii}.e_r2]);
    dyadfit_temp = fit(trials, err_mean_temp', 'exp1', 'Exclude', err_mean_temp>0.01);
    h1 = plot(dyadfit_temp, cpstring);
    set(h1,'linewidth',2);
    hold on
    err_mean_g1 = mean([err_mean_g1; experiments_struct.Experiment{1, ii}.e_r1; experiments_struct.Experiment{1, ii}.e_r2]);
end

title('Learning Curves in Group 1', 'FontSize', 20)
xlabel('Trials', 'FontSize', 20)
ylabel('Trial RMS Error[m]', 'FontSize', 20)
legend('dyad 7', 'dyad 7', 'dyad 7', 'dyad 8', 'dyad 8', 'dyad 8', 'dyad 12', 'dyad 12', 'dyad 12', 'dyad 13', 'dyad 13', 'dyad 13')
xlim([1 48])
hold off
savefig('Learning Curve Group 1')

% GROUP 2: CONNECTION STIFFNESS = 125 N/m
all_errors_g2 = [];
err_mean_g2 = [];
f = figure()
set(f, 'Color', 'w');
ccount = 1;
for ii = g2_experiments
    switch ccount
        case 1
            cstring = 'k';
            cpstring = 'k--';
        case 2
            cstring = 'b';
            cpstring = 'b--';
        case 3
            cstring = 'r';
            cpstring = 'r--';
        case 4
            cstring = 'g';
            cpstring = 'g--';
    end
    all_errors_g2 = [all_errors_g2 experiments_struct.Experiment{1, ii}.e_r1' experiments_struct.Experiment{1, ii}.e_r2'];
    plot(experiments_struct.Experiment{1, ii}.expfit_r1, cstring)
    hold on
    plot(experiments_struct.Experiment{1, ii}.expfit_r2, cstring)
    hold on
    ccount = ccount+1;
    
    % calculate mean to have 1 curve per dyad
    err_mean_temp = mean([experiments_struct.Experiment{1, ii}.e_r1; experiments_struct.Experiment{1, ii}.e_r2]);
    dyadfit_temp = fit(trials, err_mean_temp', 'exp1', 'Exclude', err_mean_temp>0.01);
    h1 = plot(dyadfit_temp, cpstring);
    set(h1,'linewidth',2);
    hold on
    err_mean_g2 = mean([err_mean_g2; experiments_struct.Experiment{1, ii}.e_r1; experiments_struct.Experiment{1, ii}.e_r2]);
end
title('Learning Curves in Group 2', 'FontSize', 20)
xlabel('Trials', 'FontSize', 20)
ylabel('Trial RMS Error [m]', 'FontSize', 20)
legend('dyad 4', 'dyad 4', 'dyad 4', 'dyad 10',  'dyad 10', 'dyad 10', 'dyad 11', 'dyad 11', 'dyad 11', 'dyad 15', 'dyad 15', 'dyad 15')
xlim([1 48])
hold off
savefig('Learning Curve Group 2')

% GROUP 3: CONNECTION STIFFNESS = 250 N/m
all_errors_g3 = [];
err_mean_g3 = [];
f = figure()
set(f, 'Color', 'w');
ccount = 1;
for ii = g3_experiments
    switch ccount
        case 1
            cstring = 'k';
            cpstring = 'k--';
        case 2
            cstring = 'b';
            cpstring = 'b--';
        case 3
            cstring = 'r';
            cpstring = 'r--';
        case 4
            cstring = 'g';
            cpstring = 'g--';
    end
    all_errors_g3 = [all_errors_g3 experiments_struct.Experiment{1, ii}.e_r1' experiments_struct.Experiment{1, ii}.e_r2'];
    plot(experiments_struct.Experiment{1, ii}.expfit_r1, cstring)
    hold on
    plot(experiments_struct.Experiment{1, ii}.expfit_r2, cstring)
    hold on
    ccount = ccount+1;
    
    % calculate mean to have 1 curve per dyad
    err_mean_temp = mean([experiments_struct.Experiment{1, ii}.e_r1; experiments_struct.Experiment{1, ii}.e_r2]);
    dyadfit_temp = fit(trials, err_mean_temp', 'exp1', 'Exclude', err_mean_temp>0.01);
    h1 = plot(dyadfit_temp, cpstring);
    set(h1,'linewidth',2);
    hold on
    
    err_mean_g3 = mean([err_mean_g3; experiments_struct.Experiment{1, ii}.e_r1; experiments_struct.Experiment{1, ii}.e_r2]);
end
title('Learning Curves in Group 3', 'FontSize', 20)
xlabel('Trials', 'FontSize', 20)
ylabel('Trial RMS Error[m]', 'FontSize', 20)
legend('dyad 5', 'dyad 5', 'dyad 5', 'dyad 6', 'dyad 6', 'dyad 6', 'dyad 9', 'dyad 9', 'dyad 9', 'dyad 14', 'dyad 14', 'dyad 14')
xlim([1 48])
hold off
savefig('Learning Curve Group 3')

% fit groups means
f = figure()
set(f, 'Color', 'w');
dyadfit_g1 = fit(trials, err_mean_g1', 'exp1', 'Exclude', err_mean_g1>0.01);
dyadfit_g2 = fit(trials, err_mean_g2', 'exp1', 'Exclude', err_mean_g1>0.01);
dyadfit_g3 = fit(trials, err_mean_g3', 'exp1', 'Exclude', err_mean_g1>0.01);
plot(solofit, 'k');
hold on
plot(dyadfit_g1, 'b');
hold on
plot(dyadfit_g2, 'g')
hold on
plot(dyadfit_g3, 'r')
grid on
title('Learning Curves by Group', 'FontSize', 20)
xlabel('Trials', 'FontSize', 20)
ylabel('Trial RMS Error[m]', 'FontSize', 20)
%legend('Solo', 'Conn.Stiff. 50 N/m', 'Conn.Stiff. 125 N/m', 'Conn.Stiff. 250 N/m')
legend('Solo', 'Group 1', 'Group 2', 'Group 3')
xlim([1 48])
savefig('Learning Curves Groups')
%% MPIR vs STIFFNESS

cs_array = [0; 50; 125; 250];
mpir_array = [solofit.b; dyadfit_g1.b; dyadfit_g2.b; dyadfit_g3.b];
f = figure()
set(f, 'Color', 'w');
plot (cs_array, abs(mpir_array), 'o-')
grid on
title('Performance Improvement by Connection Stiffness', 'FontSize', 20)
xlabel('Connection Stiffness [N/m]', 'FontSize', 20)
ylabel('Motor Performance Improvement Rate', 'FontSize', 20)
savefig('Performance Improvement by Stiffness')