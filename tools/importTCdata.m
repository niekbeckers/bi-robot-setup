function data = importTCdata(datapath, modelname)
%% importTCdata
%
% Function to load twincat data.
% Prerequisite: sort_nat (). Put this function in your matlab main folder
% (Documents/MATLAB).
%
% Niek Beckers

%% check input
% no data path given? Call UI to select folder manually
if nargin < 1
    datapath = uigetdir;
end
if nargin == 2
    % model name is given, load variable names from model
    load_system(get_param(modelname,'filename'));
    str_param_BROS1 = strsplit(get_param([modelname '/BusSelectorLoggerBROS1'],'OutputSignals'),',').';
    str_param_BROS1 = strcat(str_param_BROS1,'_BROS1');
    str_param_BROS2 = strsplit(get_param([modelname '/BusSelectorLoggerBROS2'],'OutputSignals'),',').';
    str_param_BROS2 = strcat(str_param_BROS2,'_BROS2');
else
    str_param_BROS1 = {'SystemState_BROS1';'qJointSpace.q_AbsEnc_BROS1';'qJointSpace.qdot_AbsEnc_BROS1';'qJointSpace.q_IncEnc_BROS1';'qJointSpace.qdot_IncEnc_BROS1';'xOpSpace.x_AbsEnc_BROS1';'xOpSpace.xdot_AbsEnc_BROS1';'Controller_ToLogger.x_ref_BROS1';'Controller_ToLogger.xdot_ref_BROS1';'Controller_ToLogger.xddot_ref_BROS1';'qJointSpace.JointTorque_Nm_BROS1';'BusFTSensor.ForcesOpSpace_BROS1';'BusActuators.BusActuator1.JointVelocity_LPF_rads_BROS1';'BusActuators.BusActuator2.JointVelocity_LPF_rads_BROS1'};
    str_param_BROS2 = {'SystemState_BROS2';'qJointSpace.q_AbsEnc_BROS2';'qJointSpace.qdot_AbsEnc_BROS2';'qJointSpace.q_IncEnc_BROS2';'qJointSpace.qdot_IncEnc_BROS2';'xOpSpace.x_AbsEnc_BROS2';'xOpSpace.xdot_AbsEnc_BROS2';'Controller_ToLogger.x_ref_BROS2';'Controller_ToLogger.xdot_ref_BROS2';'Controller_ToLogger.xddot_ref_BROS2';'qJointSpace.JointTorque_Nm_BROS2';'BusFTSensor.ForcesOpSpace_BROS2';'BusActuators.BusActuator1.JointVelocity_LPF_rads_BROS2';'BusActuators.BusActuator2.JointVelocity_LPF_rads_BROS2'};
end

%% load measurement data
% currentdir = [fileparts(mfilename('fullpath')) filesep];
currentdir = pwd;

cd(datapath);                                   % go to data directory

datafiles = dir('data_part*.mat');              % get list of all datafiles
filenames = sort_nat({datafiles(:).name});      % sort files (sort_nat needed)

dataArray = [];
for ii = 1:length(filenames)
    name = [filenames{ii}];
    load(name);
    paramname = strrep(strrep(name,'part',''),'.mat','');
    eval(['data_temp = ' paramname ';']);
    dataArray = [dataArray data_temp];
    clear data_*
end

cd(currentdir); % go back to current directory
dataArray = dataArray';       % to columns

% these are the parameter labels in the recorded data
param_lbls = ['time';
    str_param_BROS1;
    str_param_BROS2;
    'target_BROS1';
    'target_BROS2';
    'cursor_BROS1';
    'cursor_BROS2';
    'Error_BROS1';
    'Error_BROS2';
    'ExpTrialNumber';
    'ExpTrialRunning'];   

% indices corresponding to each parameter
param_idx = {1; 2; 3:4; 5:6; 7:8; 9:10; 11:12; 13:14; 15:16; 17:18; 19:20; 21:22; 23:28; 29; 30;...
               31; 32:33; 34:35; 36:37; 38:39; 40:41; 42:43; 44:45; 46:47; 48:49; 50:51; 52:57; 58; 59;...
               60:61; 62:63; 64:65; 66:67; 68; 69; 70; 71};


% create struct with all data parameters
data = struct;
for ii = 1:length(param_lbls)
    param = param_lbls{ii};
    if ~isempty(strfind(param,'.')) 
        if strcmpi(version('-release'),'2016b')
            param = extractAfter(param,'.'); 
        else
            ix = strfind(param,'.');
            param = param(ix(end)+1:end);
        end
    end
    
    eval(['data.' char(param) ' = dataArray(:,param_idx{ii});']);
end