function dataraw = loadBROSdata(datapath, savepath)

% check input parameters
if nargin < 2
    savepath = '.';
end

% import data
dataraw = importTCdata(datapath);
trialnumbers = unique(dataraw.ExpTrialNumber);

% get the variable names. Remove 'BusActuator1', 'BusActuator2' and 't'
vars = {'q_AbsEnc_BROS1','q_AbsEnc_BROS2','qdot_AbsEnc_BROS1','qdot_AbsEnc_BROS2','x_AbsEnc_BROS1','x_AbsEnc_BROS2','xdot_AbsEnc_BROS1','xdot_AbsEnc_BROS2','ForcesOpSpace_BROS1','ForcesOpSpace_BROS2','target_BROS1','target_BROS2','cursor_BROS1','cursor_BROS2'}; 

% define data struct
data = struct;

for ii = 1:length(trialnumbers)
    idxtrial = findseq(double(dataraw.ExpTrialNumber == trialnumbers(ii) & dataraw.ExpTrialRunning));
    idx = idxtrial(1,2):idxtrial(1,3);
    
    % make own time vector per trial
    t = dataraw.time(idx); t = t - t(1);
    data.trial(ii).t = t;
    
    % select the data per trial
    for jj = 1:length(vars)
        data.trial(ii).(vars{jj}) = dataraw.(vars{jj})(idx,:);
    end
end

% save data to mat file
save([savepath filesep 'data_trials.mat'],'data');


