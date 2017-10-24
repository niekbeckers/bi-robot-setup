
function alldata = loadBROSExperimentData(datapath, savepath, vars)
%% function dataraw = loadBROSdata(datapath, savepath)
%
% Load relevant data from the massive TwinCAT data files. 
% You can adapt the 'vars' parameter to your liking. Note that the variable
% name you want to store in the 'data' struct need to match the
% variablename in the 'dataraw' struct.
%
% Niek Beckers, 2017

% check input parameters
if nargin < 2 || isempty(savepath)
    % save in same folder as raw data
    savepath = datapath;
end

if nargin < 3
    % get the variable names. Remove 'BusActuator1', 'BusActuator2' and 't'
    vars = {'x_BROS1','x_BROS2','xdot_BROS1','xdot_BROS2',...
            'x_AbsEnc_BROS1','x_AbsEnc_BROS2','xdot_AbsEnc_BROS1','xdot_AbsEnc_BROS2',...
            'ForcesOpSpace_BROS1','ForcesOpSpace_BROS2',...
            'target_BROS1','target_BROS2','cursor_BROS1','cursor_BROS2','target_vel_BROS1','target_vel_BROS2'}; 
end

dt = 0.001;

% import data
alldata = importTCdata(datapath);

trialnumbers = unique(alldata.ExpTrialNumber);



% check how many sequences of trial running we have. This number should be
% the same as the length of trialnumber. If not, probably this is due to
% restarting the experiment after an error or something. Fix it here.
trialrunning = findseq(double(alldata.ExpTrialRunning));
idxtrialrunning = find(trialrunning(:,1)==1);

if length(trialnumbers) ~= length(idxtrialrunning)
    
   warning('Inconsistent trialnumbers and dataraw.ExpTrialNumbers. Fixing it.');
   trialnumbers = 1:length(idxtrialrunning);
   
   % also fix dataraw.ExpTrialNumber (note: we are not overwriting the
   % actual data files, just the variable)
   for ii = 1:length(idxtrialrunning)
       idx = trialrunning(idxtrialrunning(ii),2):trialrunning(idxtrialrunning(ii),3);
       alldata.ExpTrialNumber(idx) = trialnumbers(ii);
   end
end



% define data struct
data = struct;

for ii = 1:length(trialnumbers)
    idxtrial = findseq(double(alldata.ExpTrialNumber == trialnumbers(ii) & alldata.ExpTrialRunning));
    idx = idxtrial(1,2):idxtrial(1,3);
    
    % make own time vector per trial
    t = alldata.time(isdx); t = t - t(1);
    data.trial(ii).t = t;
    
    % retrieve dt (assume it's a multiple of 1ms)
    dt = round(mode(diff(t))/dt)*dt;

    % select the data per trial
    for jj = 1:length(vars)
        try
%             [tres,datares]= resampleTCdata(t,alldata.(vars{jj})(idx,:),dt);
            data.trial(ii).(vars{jj}) = alldata.(vars{jj})(idx,:);
            data.trial(ii).t = t; 
        catch me
            disp(getReport( me, 'extended', 'hyperlinks', 'on' ));
        end
    end
end

% save data to mat file
save([savepath filesep 'data_trials.mat'],'data');


