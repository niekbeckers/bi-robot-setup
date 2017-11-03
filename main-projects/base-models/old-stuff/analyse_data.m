%% analyse_transmission_ratio
% Estimate transmission ration using regression, by comparing the absolute
% encoder and motor encoder angles.

clear all; close all;

%% load measurement data
data = [];

currentdir = [fileparts(mfilename('fullpath')) filesep];
% datadir = '../data/affc/meas3_affc_velfilter/';
% datadir = '../data/affc/meas2_affc/';
datadir = '../data/base-1fm/meas6_slower_comp/';
str_title = 'measurement 2';

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
tau_y = data(ix,23:24);
tau_nlcomp = data(ix,25:26);
tau_des = tau_y+tau_nlcomp;

e = q-q_ref;



%% plot


% plot position and velocity 
figure('color','w','position',[0 0 370 600])
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

figure
subplot(311)
plot(t,tau_y); hold on
subplot(312)
plot(t,tau_nlcomp);
subplot(313)
plot(t,tau_des);


figure
plot(t,e)

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