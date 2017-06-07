
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
if isempty(datapath)
    datapath = uigetdir;
end

%% load measurement data
% currentdir = [fileparts(mfilename('fullpath')) filesep];
currentdir = pwd;

cd(datapath); % go to data directory

datafiles = dir('data_part*.mat');   % get list of all datafiles
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

cd(currentdir);
dataArray = dataArray';       % to columns

% load and prepare variable names
load_system(get_param(modelname,'filename'));
str_param_BROS1 = strsplit(get_param([modelname '/BusSelectorLoggerBROS1'],'OutputSignals'),',').';
str_param_BROS1 = strcat(str_param_BROS1,'_BROS1');
str_param_BROS2 = strsplit(get_param([modelname '/BusSelectorLoggerBROS2'],'OutputSignals'),',').';
str_param_BROS2 = strcat(str_param_BROS2,'_BROS2');

% these are the parameter labels in the recorded data
param_lbls = ['time';str_param_BROS1;str_param_BROS2;'target_BROS1';'target_BROS2';'cursor_BROS1';'cursor_BROS2';'Error_BROS1';'Error_BROS2';'ExpTrialNumber';'ExpTrialRunning'];   

% indices corresponding to each parameter
param_idx = {1; 2; 3:4; 5:6; 7:8; 9:10; 11:12; 13:14; 15:16; 17:18; 19:20; 21:22; 23:28; 29; 30;...
               31; 32:33; 34:35; 36:37; 38:39; 40:41; 42:43; 44:45; 46:47; 48:49; 50:51; 52:57; 58; 59;...
               57:58; 59:60; 61:62; 63:64; 65; 66; 67; 68};

% create struct with all data parameters
data = struct;
for ii = 1:length(param_lbls)
    param = param_lbls{ii};
    if ~isempty(strfind(param,'.')), param = extractAfter(param,'.'); end
    
    eval(['data.' char(param) ' = dataArray(:,param_idx{ii});']);
end