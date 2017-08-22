%% analyse_transmission_ratio
% Estimate transmission ration using regression, by comparing the absolute
% encoder and motor encoder angles.

clear; close all; clc;

%% load measurement data
data = [];

currentdir = [fileparts(mfilename('fullpath')) filesep];
datadir = '../data/transmission-ratio/meas-fm2-2/';

str_suptitle = 'Transmission Ratio FM2';
filename_str = 'transmission_data';

cd(datadir)
datafiles = dir([filename_str '_part*.mat']);

for ii = 1:length(datafiles)
    load([datafiles(ii).name]);
    name = datafiles(ii).name;
    num = str2double(name(strfind(name,'part')+4:end-3));
    eval(['data_temp = data_' num2str(num) ';']);
    data = [data data_temp];
    clear data_*
end

cd(currentdir);
data = data.';

data = data(5000:end,:);
%% extract data
t = data(:,1);
pos_motorenc1 = data(:,2);  
pos_absenc1 = data(:,3);    
pos_motorenc2 = data(:,4);  
pos_absenc2 = data(:,5);    

% demean
pos_motorenc1 = pos_motorenc1 - mean(pos_motorenc1);
pos_absenc1 = pos_absenc1 - mean(pos_absenc1);
pos_motorenc2 = pos_motorenc2 - mean(pos_motorenc2);
pos_absenc2 = pos_absenc2 - mean(pos_absenc2);

%% use regression to estimate transmission ratio

% joint 1
[b1,~,~,~,stats1] = regress(pos_motorenc1, [ones(size(pos_absenc1)) pos_absenc1]);

% calculate fit
pos_absenc1_fit = (pos_motorenc1-b1(1))/b1(2);

% joint 2
% use regression to fit transmission ratio
[b2,~,~,~,stats2] = regress(pos_motorenc2, [ones(size(pos_absenc2)) pos_absenc2]);

% calculate fit
pos_absenc2_fit =  (pos_motorenc2 - b2(1))/b2(2);

%% plot results
figure('color','w','position',[0 0 600 600]);

subplot(211); title('Transmission ratio 1')
plot(t,pos_absenc1,t,pos_motorenc1); hold on;
plot(t,pos_absenc1_fit,'--');
ylabel('Position, rad'); xlabel('Time, s')
text(0.02,0.06,['transmissionRatio = ' num2str(b1(2),'%.5f')],'units','normalized');
legend({'Abs Enc','Inc Enc',['Fit: R^2 = ' num2str(stats1(1),'%.2f')]},'FontSize',8)

subplot(212); title('Transmission ratio 2')
plot(t,pos_absenc2,t,pos_motorenc2); hold on;
plot(t,pos_absenc2_fit,'--');
ylabel('Position, rad'); xlabel('Time, s')
text(0.02,0.06,['transmissionRatio = ' num2str(b2(2),'%.5f')],'units','normalized');
legend({'Abs Enc','Inc Enc',['Fit: R^2 = ' num2str(stats2(1),'%.2f')]},'FontSize',8);

suptitle(str_suptitle)

print([datadir '..\images\tranmission_ratio_estimation_fm1.png'],'-dpng','-r300')