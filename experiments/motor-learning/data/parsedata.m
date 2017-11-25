function varargout = parsedata;

%% parsedata
clear all; close all; clc;

% pairs = [1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20];
pairs = 1:20;
sessions = [1 2];
% outliers = {'10B'};
outliers = {''};
switchers = [1 8];

ix = 1;
currdir = [fileparts(mfilename('fullpath')) filesep];

for ii = 1:length(pairs)
    pair = pairs(ii);
    for jj = 1:length(sessions)
        session = sessions(jj);
        disp(['Parsing pair ' num2str(pair) ' session ' num2str(session)]);
        try
            % load data and experiment protocol
            f = dir(['.' filesep 'exp1_motorlearning_partners' num2str(pair) '_type*']);
            cd([f(1).folder filesep f(1).name filesep 'session' num2str(session)])
            if exist('data_trials.mat','file')
                load('data_trials.mat');
            else 
                disp('data_trials.mat not found, calling loadBROSExperimentData');
                loadBROSExperimentData('.');
            end
            
            f = dir(['.' filesep '..' filesep 'expprotocol_motorlearning_partners' num2str(pair) '_session' num2str(session) '*.mat']);
            load([f(1).folder filesep f(1).name]);
            cd(currdir);
            
            % calculate performance metrics
            clear alldatatmp
            alldatatmp = calculate_errorparams(data, expprotocol, pair,'Tpart',5);
            
            % remove outliers
            alldatatmp = removeoutliers(alldatatmp,pair,outliers);
            
            % switch users (soem users sat behind BROS 1 for session 1 and
            % switched to BROS2 for session 2). 
            alldatatmp = switchbros(alldatatmp,pair,session,switchers);
            
            alldatatmp.areConnected = any(any([expprotocol.block(:).connected] == 1));
            alldatatmp.pair = pair;
            alldatatmp.session = session;

            alldata(ix) = alldatatmp;
            ix = ix + 1;
        catch me
            fprintf('Caught error for pair %d, session %d\n',pair,session)
            disp(getReport(me))
            cd(currdir);
        end
    end
end   
clear alldatatmp;
cd(currdir);

% save data
save('HHILearningData.mat','alldata');

varargout{1} = alldata;

end

function data = removeoutliers(data,pair,outliers)
%% function data = removeoutliers(data,pair,outliers)
% set data of outlier to NaN

partners = {'A','B'};
for ii = 1:2
    id = [num2str(pair) partners{ii}];
    if any(ismember(id,outliers))
        disp(['Outlier: data for ' num2str(id) ' set to NaN']);
        data = setdatatonan(data,ii);
    end 
end

end

function data = setdatatonan(data,pidx)
%% function data = setdatatonan(data,ii)
% set data corresponding to pidx (partner index, 1 or 2) to NaN for all
% fields in the struct. Also does this recursively for sub-structs in the
% main struct.
fldsdonotnan = {'idx_partner','idx_blocks','idx_trials'};

fldnms = fieldnames(data);
for jj = 1:length(fldnms)
    if ~ismember(fldnms{jj},fldsdonotnan)
        tmp = data.(fldnms{jj});
        if isstruct(tmp)
            for kk = 1:length(tmp)
                tmp(kk) = setdatatonan(tmp(kk),pidx); % recursion, woot!
            end
        else
            if ~any(islogical(tmp)) % cannot set logicals to NaN
                tmp(:,pidx:2:end) = NaN;
            end
        end
        data.(fldnms{jj}) = tmp;
    end
end

end

function data = switchbros(data,pair,session,switchers)
%% function data = removeoutliers(data,pair,outliers)
% set data of outlier to NaN

if ismember(pair, switchers) && (session==2)
    disp(['switchbros: data for ' num2str(pair) ' switched BROS1 and BROS2 for session ' num2str(session)]);
    data = switchBROS1and2(data); % switch BROS1 and BROS2
end

end

function data = switchBROS1and2(data)
fldsdonotswitch = {'idx_partner','idx_blocks','idx_trials'};
fldnms = fieldnames(data);
for ii = 1:length(fldnms)
    if ~ismember(fldnms{ii}, fldsdonotswitch)
        fld = data.(fldnms{ii});
        if isstruct(fld)
            for jj = 1:length(fld)
                newdata(jj) = switchBROS1and2(fld(jj));
            end
        else
            newdata = NaN(size(fld));
            newdata(:,1:2:end) = fld(:,2:2:end);
            newdata(:,2:2:end) = fld(:,1:2:end);
        end
        data.(fldnms{ii}) = newdata;
    end
end

end