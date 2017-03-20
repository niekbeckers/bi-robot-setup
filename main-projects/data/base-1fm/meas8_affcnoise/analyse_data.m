%% analyse_transmission_ratio
% Estimate transmission ration using regression, by comparing the absolute
% encoder and motor encoder angles.

clear all; close all;

%% load measurement data
data = [];

currentdir = [fileparts(mfilename('fullpath')) filesep];
% datadir = '../data/affc/meas3_affc_velfilter/';
% datadir = '../data/affc/meas2_affc/';
datadir = '.';
str_title = 'Measurement AFFC - noise';

filename_str = 'data';

cd(datadir)
mask_files = dir([filename_str '_part*.mat']);
files = cell(cellfun(@(x)x,{mask_files.name},'UniformOutput',false))';
files = sort_nat(files);

for ii = 1:length(files)
    name = files{ii};
    ix1 = strfind(name,'part');
    ix2 = strfind(name,'.mat');
    num = str2double(name(ix1(1)+4:ix2(1)-1));
    load(files{ii});
    eval(['data_temp = data_' num2str(num) ';']);
    data = [data data_temp];
    clear data_*
end

cd(currentdir);
data = data';

%% extract data

systemstate = data(:,2);

% select a states
% ix = systemstate == 4;
ix = 1:length(systemstate); % == 302;

t = data(ix,1); 
dt = mode(diff(t));
q = data(ix,3:4);
qdot = data(ix,5:6);
% x = data(ix,7:8);
% xdot = data(ix,9:10);
% q_ref = data(ix,11:12);
% qdot_ref = data(ix,13:14);
% 
% % affc param vector
% tau_y = data(ix,23:24);
% tau_nlcomp = data(ix,25:26);
% tau_des = tau_y+tau_nlcomp;
% 
% e = q-q_ref;
qdot_lpf = data(ix,29:30);
qdiff = interp1(t(1:end-1)+0.5*dt, diff(q)/dt, t);



%% plot


% plot position and velocity 
figure('color','w','position',[0 0 370 600])
subplot(211)
plot(t, q(:,1),'-');
% legend('actual','Location','SouthWest')
% xlabel('$q_1$, rad','interpreter','latex'); ylabel('$q_2$, rad','interpreter','latex');
% axis square

subplot(212)
plot(t,qdiff(:,1),'-','color',[0.8 0.8 0.8]); hold on
plot(t,qdot(:,1),'-'); 
plot(t,qdot_lpf(:,1)); 

legend({'diff','foaw','lpf'})
text(10,-12,'K_{FOAW} = 1','FontSize',8,'HorizontalAlignment','center');
text(24,-12,'K_{FOAW} = 2','FontSize',8,'HorizontalAlignment','center');
text(36,-12,'K_{FOAW} = 4','FontSize',8,'HorizontalAlignment','center');
text(48,-12,'K_{FOAW} = 8','FontSize',8,'HorizontalAlignment','center');
text(59,-12,'K_{FOAW} = 10','FontSize',8,'HorizontalAlignment','center');
text(71,-12,'K_{FOAW} = 16','FontSize',8,'HorizontalAlignment','center');
text(83,-12,'K_{FOAW} = 32','FontSize',8,'HorizontalAlignment','center');
% xlabel('$\dot{q}_1$, rad','interpreter','latex'); ylabel('$\dot{q}_2$, rad','interpreter','latex');
% axis square
% suptitle(['Kinematic results, ' str_title])


% figure
% subplot(311)
% plot(t,tau_y); hold on
% subplot(312)
% plot(t,tau_nlcomp);
% subplot(313)
% plot(t,tau_des);
% 
% 
% figure
% plot(t,e)

% print([datadir '..\images\affc_results_kinematics_fm1_' strrep(str_title,' ','') '.png'],'-dpng','-r300')

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