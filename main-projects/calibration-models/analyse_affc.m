%% analyse_transmission_ratio
% Estimate transmission ration using regression, by comparing the absolute
% encoder and motor encoder angles.

clear all; close all;

%% load measurement data
data = [];

currentdir = [fileparts(mfilename('fullpath')) filesep];
% datadir = '../data/affc/meas3_affc_velfilter/';
% datadir = '../data/affc/meas2_affc/';
datadir = '../data/affc/meas03_bros2/';
str_title = 'meas03_bros2';

filename_str = 'data';

cd(datadir)
datafiles = dir([filename_str '_part*.mat']);
filenames = sort_nat({datafiles(:).name});

for ii = 1:length(filenames)
    name = [filenames{ii}];
    load(name);
    paramname = strrep(strrep(name,'part',''),'.mat','');
    eval(['data_temp = ' paramname ';']);
    data = [data data_temp];
    clear data_*
end

cd(currentdir);
data = data';

%% extract data
systemstate = data(:,2);

% select a states
ix = systemstate == 4;
% ix = 1:length(systemstate); % == 302;

t = data(ix,1); 

q = data(ix,3:4);
qdot = data(ix,5:6);
x = data(ix,7:8);
xdot = data(ix,9:10);
q_ref = data(ix,11:12);
qdot_ref = data(ix,13:14);

% affc param vector
p = data(ix,17:23);

% filter the parameter vector
[B,A] = butter(2, 0.2/500);
pfilt = filtfilt(B,A,p);

%% plot

% plot parametes
figure('color','w','defaulttextinterpreter','latex');
ylabels = {'$I_1$, $kgm^2$]','$I_2$, $kgm^2$','$m_e$, $kg$','$F_{s_1}$, $Nm$','$F_{d_1}$, $Nm$','$F_{s_2}$, $Ns/m$','$F_{d_2}$, $Ns/m$'};
subplotpos = [1 2 3 5 7 6 8];

p_hat = mean(pfilt(end-5000-20000:end-20000,:),1);

% print to command window for easy copy paste
disp(['[' num2str(p_hat,'%e ') ']'])

for ii = 1:7
    subplot(4,2,subplotpos(ii))
    plot(t,p(:,ii)); hold on;
    plot(t,pfilt(:,ii),'--','linewidth',2,'color',0.5*[1 1 1]);
    xlabel('t, s','interpreter','latex'); ylabel(ylabels{ii},'interpreter','latex');
    text(1,0,{['$w_{est} = ' num2str(p_hat(ii),'%1.4e') '$']},'interpreter','latex','VerticalAlignment','Bottom','HorizontalAlignment','Right','Units','Normalized')
end
suptitle(['AFFC - estimated parameters, ' str_title]);
print([datadir '..\images\affc_results_params_' strrep(str_title,' ','') '.png'],'-dpng','-r300')


% plot position and velocity 
figure('color','w','position',[0 0 370 600],'defaulttextinterpreter','latex')
subplot(211)
plot(q_ref(:,1),q_ref(:,2),'--','color',[0.8 0.8 0.8]); hold on;
plot(q(:,1),q(:,2),'-');
legend('ref','actual','Location','SouthWest')
xlabel('$q_1$, rad','interpreter','latex'); ylabel('$q_2$, rad','interpreter','latex');
axis square

subplot(212)
plot(qdot_ref(:,1),qdot_ref(:,2),'--','color',[0.8 0.8 0.8]); hold on;
plot(qdot(:,1),qdot(:,2),'-');
xlabel('$\dot{q}_1$, rad','interpreter','latex'); ylabel('$\dot{q}_2$, rad','interpreter','latex');
axis square
suptitle(['Kinematic results, ' str_title])


print([datadir '..\images\affc_results_kinematics_' strrep(str_title,' ','') '.png'],'-dpng','-r300')

% % operational space
% Lf = 0.238;                     % forearm length [m]
% Lu = 0.153;                     % upperarm length [m]
% Lb = 0.07;                      % base width [m]
% lala= 1
% x_ref = func_eq_FK(q_ref(:,1)',q_ref(:,2)',Lf,Lu,Lb)';
% figure('color','w','position',[0 0 370 600])
% 
% subplot(211)
% plot(x_ref(:,1),x_ref(:,2),'--','color',[0.8 0.8 0.8]); hold on;
% plot(x(:,1),x(:,2),'-');
% legend('ref','actual','Location','SouthWest')
% xlabel('$q_1$, rad','interpreter','latex'); ylabel('$q_2$, rad','interpreter','latex');
% axis square
% 
% subplot(212)
% plot(qdot_ref(:,1),qdot_ref(:,2),'--','color',[0.8 0.8 0.8]); hold on;
% plot(xdot(:,1),xdot(:,2),'-');
% xlabel('$\dot{q}_1$, rad','interpreter','latex'); ylabel('$\dot{q}_2$, rad','interpreter','latex');
% axis square