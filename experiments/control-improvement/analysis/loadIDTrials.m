%% loadIDTrials
clear all; close all; clc;
runnr = 15;
load(['data_x_nom_run' num2str(runnr) '.mat']);

% select ID trials
trialrunning = findseq(double(data.ExpTrialRunning));

idxtrialrunning = find((trialrunning(:,1) == 1) ); % & (trialrunning(:,4) > 10)

trialdata = struct;
fldnms = fieldnames(data);
for ii = 1:length(idxtrialrunning)
    idxtrial = trialrunning(idxtrialrunning(ii),:);
    idx = idxtrial(1,2):idxtrial(1,3);
    idx = idx(end-12000+1:end);
    
    for jj = 1:length(fldnms)
        trialdata.trial(ii).(fldnms{jj}) = data.(fldnms{jj})(idx,:);
    end
    
end

save(['data_sysid_x_nom_run' num2str(runnr) '.mat'],'trialdata');