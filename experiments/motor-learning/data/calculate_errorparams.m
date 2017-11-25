function dataout = calculate_errorparams(datain, expprotocol, pairID, varargin)
%% calculate experiment parameters:
%
% Data structure
% Data (RMSe, improvement, relative performance) is parsed into blocks 


% parse inputs
p = inputParser;
p.addOptional('Tpart',5);
p.addOptional('Nsel',20000);
p.addOptional('dt',0.001);
p.addOptional('AlignTrials',false);
p.parse(varargin{:});

% select inputparser results
Nsel = p.Results.Nsel;
Tpart = p.Results.Tpart;
dt = p.Results.dt;

% also chop trials up in parts
Nparts = Nsel*dt/Tpart; % divide error over 2 seconds

% determine which trials are connected connected
connected = [expprotocol.block.connected].*([expprotocol.block.connectionStiffness] > 0);
Nblocks = size(connected,2);

% trial indices (in blocks)
idx_trials = reshape(1:numel(connected),size(connected));
idx_connected = reshape(connected == 1,size(connected));
idx_single = reshape(connected == 0,size(connected));

% preallocate
rmse1 = NaN(size(connected));
rmse2 = NaN(size(connected));
rmse1_parts = NaN([size(connected) Nparts]);
rmse2_parts = NaN([size(connected) Nparts]);
blocks = struct;

for ii = 1:Nblocks % blocks
    
    % per trial and partner, calculate RMS
    for jj = 1:size(idx_trials,1)
        idxtrial = idx_trials(jj,ii);
        dat = datain.trial(idxtrial);
        
        % select target and cursor data
        target = NaN(Nsel,4);
        cursor = NaN(Nsel,4);
        
        % number of data points, time step
        N = size(dat.target_BROS1,1);
        if (N > Nsel)
            idx = N-Nsel+1:N;
        else
            idx = 1:N;
        end

        target(end-length(idx)+1:end,:) = [dat.target_BROS1(idx,:) dat.target_BROS2(idx,:)];
        cursor(end-length(idx)+1:end,:) = [dat.cursor_BROS1(idx,:) dat.cursor_BROS2(idx,:)];
        
        if p.Results.AlignTrials
            % shift target and cursor data such that for each trial they are
            % the same (i.e. trialRandomization to 0)
            Tshift = expprotocol.block(ii).trialRandomization(jj);
            Tshift = round(Tshift/dt)*dt;
            Nshift = round(Tshift/dt);
            target = circshift(target,Nshift,1);
            cursor = circshift(cursor,Nshift,1);
        end
        
        % calculate RMS
        [rmse1(jj,ii),rmse1_parts(jj,ii,:),idx_trials_parts] = calculate_rmse(target(:,1:2),cursor(:,1:2),idx_trials,Nparts);
        [rmse2(jj,ii),rmse2_parts(jj,ii,:)]                  = calculate_rmse(target(:,3:4),cursor(:,3:4),idx_trials,Nparts);
    end
    
    % fix some errors (due to TC errors/restarts)
    [idx_connected(:,ii),idx_single(:,ii),rmse1(:,ii),rmse2(:,ii),rmse1_parts(:,ii,:),rmse2_parts(:,ii,:)] = ...
        fixerrors(pairID,ii,idx_connected(:,ii),idx_single(:,ii),rmse1(:,ii),rmse2(:,ii),rmse1_parts(:,ii,:),rmse2_parts(:,ii,:));
    
    if ~any(idx_connected(:)==1)
        idx_connected(2:2:end,:) = true;
        idx_single(2:2:end,:) = false;
    end
    
    % store data per block
    blocks(ii).rmse = [rmse1(:,ii) rmse2(:,ii)];
    blocks(ii).rmse_single = [rmse1(idx_single(:,ii),ii) rmse2(idx_single(:,ii),ii)];
    blocks(ii).rmse_connected = [rmse1(idx_connected(:,ii),ii) rmse2(idx_connected(:,ii),ii)];
    blocks(ii).idx_trials = repelem(idx_trials(:,ii),1,2); % relative idx_trial indices
    blocks(ii).idx_trials_connected = repelem(idx_trials(idx_connected(:,ii),ii),1,2); % relative idx_trial_connected indices
    blocks(ii).idx_trials_single = repelem(idx_trials(idx_single(:,ii),ii),1,2); % relative idx_trial indices
    blocks(ii).idx_connected = repelem(idx_connected(:,ii),1,2);
    blocks(ii).idx_single = repelem(idx_single(:,ii),1,2);

    rmse_parts_tmp = cat(3,permute(rmse1_parts(:,ii,:),[3 1 2]),permute(rmse2_parts(:,ii,:),[3 1 2]));
    blocks(ii).rmse_parts = reshape(rmse_parts_tmp,[],2);
    blocks(ii).rmse_connected_parts = reshape(rmse_parts_tmp(:,idx_connected(:,ii),:),[],2);
    blocks(ii).rmse_single_parts = reshape(rmse_parts_tmp(:,idx_single(:,ii),:),[],2);
    idx_trials_parts_tmp = permute(idx_trials_parts(:,ii,:),[3 1 2]);
    blocks(ii).idx_trials_parts = repmat(reshape(idx_trials_parts_tmp,[],1),1,2);
    blocks(ii).idx_trials_connected_parts = repmat(reshape(idx_trials_parts_tmp(:,idx_connected(:,ii),:),[],1),1,2);
    blocks(ii).idx_trials_single_parts = repmat(reshape(idx_trials_parts_tmp(:,idx_single(:,ii),:),[],1),1,2);

    % calculate HHI improvement and relative performance
    idx_rows_connected = blocks(ii).idx_trials_connected(:,1) - blocks(ii).idx_trials(1) + 1;
    idx_rows_single = idx_rows_connected + 1;
    [blocks(ii).improvement_hhi, blocks(ii).relperformance_hhi] = calculate_hhi_improvement(blocks(ii).rmse,idx_rows_connected,idx_rows_single);
    
    rmse0 = blocks(ii).rmse_single(1,:);
    if ii == 3
        rmse0 = blocks(2).rmse_single(1,:);
    end
    blocks(ii).rmse_single_improvement = repmat(rmse0,size(blocks(ii).rmse_single,1),1) - blocks(ii).rmse_single;
    blocks(ii).rmse_single_parts_improvement = repmat(rmse0,size(blocks(ii).rmse_single_parts,1),1) - blocks(ii).rmse_single_parts;
end

% store all data
% dataout.subjID = subjID;
dataout.blocks = blocks;
dataout.rmse = [blocks(:).rmse];
dataout.rmse_single = [blocks(:).rmse_single];
dataout.rmse_connected = [blocks(:).rmse_connected];
dataout.idx_trials = repelem(idx_trials,1,2);
dataout.idx_trials_connected = [blocks(:).idx_trials_connected];
dataout.idx_trials_single = [blocks(:).idx_trials_single];
dataout.idx_connected = repelem(idx_connected,1,2);
dataout.idx_single = repelem(idx_single,1,2);
dataout.idx_blocks = repelem(repmat(1:Nblocks,size(connected,1),1),1,2);
dataout.idx_partner = repmat(repmat(1:2,size(connected,1),1),1,Nblocks);

% parts
dataout.rmse_parts = [blocks(:).rmse_parts];
dataout.rmse_connected_parts = [blocks(:).rmse_connected_parts];
dataout.rmse_single_parts = [blocks(:).rmse_single_parts];
dataout.idx_trials_parts = [blocks(:).idx_trials_parts];
dataout.idx_trials_connected_parts = [blocks(:).idx_trials_connected_parts];
dataout.idx_trials_single_parts = [blocks(:).idx_trials_single_parts];

% improvement (all, force field (blocks 2 & 3), baseline (block 1), washout (block 4)
dataout.improvement_hhi = [blocks(:).improvement_hhi];
dataout.relperformance_hhi = [blocks(:).relperformance_hhi];

% improvement learning
% subtract the initial performance for baseline, force field and washout
rmse0 = dataout.rmse_single(1,:);
rmse0(5:6) = rmse0(3:4);
dataout.rmse_single_improvement = repmat(rmse0,size(dataout.rmse_single,1),1) - dataout.rmse_single;
dataout.rmse_single_parts_improvement = repmat(rmse0,size(dataout.rmse_single_parts,1),1) - dataout.rmse_single_parts;
dataout.rmse_improvement = repmat(rmse0,size(dataout.rmse,1),1) - dataout.rmse;
end

function [rmse,rmse_part,idx_trialsparts] = calculate_rmse(t,x,idx_trials,Nparts)
%% [rmse,rmse_part,idx_trialsparts] = calculate_rmse(t,x,idx_trials,Nparts)
% calculate root-mean-squared (RMS) of the tracking error as a performance
% measure across the complete trial or the trial chopped into Nparts equal parts

% RMSe across complete trial
e = sqrt(sum((t-x).^2,2));
rmse = rms(e);

% calculate RMS across trial in equal parts
e_parts = reshape(e,[round(length(e)/Nparts) Nparts]);
rmse_part = rms(e_parts,1);

% trial numbers
idx_trialsparts = NaN([size(idx_trials),Nparts]);
for ii = 1:Nparts
    idx_trialsparts(:,:,ii) = idx_trials+(ii-1)*(1/Nparts);
end

end

function [idx_connected,idx_single,rmse1,rmse2,rmse1_parts,rmse2_parts] = fixerrors(subjnr,blocknr,idx_connected,idx_single,rmse1,rmse2,rmse1_parts,rmse2_parts)
%% function [idx_connected,idx_single,rmse1,rmse2,rmse1_parts,rmse2_parts] = fixerrors(subjnr,idx_connected,idx_single,rmse1,rmse2,rmse1_parts,rmse2_parts)

% subject 1
if (subjnr == 1) && (sum(idx_connected) > 10)
    % if per block more than 10 connected trials are found, I made an
    % error in the experiment protocol. 
    idx_connected = [idx_connected(2:end); false];
    idx_single = [idx_single(2:end); true];
    % fix the rmse -  set data points to NaN
    rmse1 = [rmse1(2:end); NaN];
    rmse2 = [rmse2(2:end); NaN];
    rmse1_parts = [rmse1_parts(2:end,:,:); NaN(size(rmse1_parts(end-1,:,:)))];
    rmse2_parts = [rmse2_parts(2:end,:,:); NaN(size(rmse2_parts(end-1,:,:)))];
end

% pair 18: trial 1 of block 2, session 1 started before we were back at the
% controls. Use data from trial 2, sicne this was the actual first trial.
% (solo pair)
if (subjnr == 18) && (blocknr == 2) && (rmse1(1) > 0.02) && (rmse2(1) > 0.02)
    rmse1(1) = rmse1(2);
    rmse2(1) = rmse2(2);
end

end

