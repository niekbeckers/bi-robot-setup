function varargout = parsedata

%% parsedata
clear all; close all; clc;

pairs = [1:3]; % pair 6 is incorrect (wrong TC model)
outliers = {''};

ix = 1;
currdir = [fileparts(mfilename('fullpath')) filesep];

vars = {'time','x_BROS1','x_BROS2','target_BROS1','target_BROS2','cursor_BROS1','cursor_BROS2','dist_pos_BROS1','dist_pos_BROS2','ForcesOpSpace_BROS1','ForcesOpSpace_BROS2','Fs1','Fs2'};


for ii = 1:length(pairs)
    pair = pairs(ii);
    disp(['Parsing pair ' num2str(pair)]);
    try
        % load data and experiment protocol
        f = dir(['.' filesep 'exp3_id_pair' num2str(pair) '*']);

        cd([f(1).folder filesep f(1).name filesep 'data'])
        if exist('data_trials.mat','file')
            load('data_trials.mat');
        else 
            disp('data_trials.mat not found, calling loadBROSExperimentData');
            data = loadBROSExperimentData('.','vars',vars);
        end

        f = dir(['.' filesep '..' filesep 'protocol_pilot_2d_pursuit_id_pair0_type*.mat']);
        load([f(1).folder filesep f(1).name]);
        cd(currdir);

        % calculate performance metrics
        clear alldatatmp
        alldatatmp = calculate_parameters(data, expprotocol);

        % remove outliers
        alldatatmp = removeoutliers(alldatatmp,pair,outliers);

        alldatatmp.areConnected = any(any([expprotocol.block(:).connected] == 1));
        alldatatmp.pair = pair;
        alldatatmp.session = 1;

        alldata(ix) = alldatatmp;
        ix = ix + 1;
    catch me
        fprintf('Caught error for pair %d\n',pair)
        disp(getReport(me))
        cd(currdir);
    end
end   
clear alldatatmp;
cd(currdir);

% save data
save('Exp3RawData.mat','alldata');

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