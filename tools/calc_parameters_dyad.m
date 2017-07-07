function [e_1, improv_single_trial_1, improv_dual_trial_1, e_2, improv_single_trial_2, improv_dual_trial_2, relative_performance1] = calc_parameters_dyad (xtarget_1, xcursor_1, xtarget_2, xcursor_2, trial_sequence) %will need more to compute improvements
%
% IN
%
%
% xtarget is the matrix containing position of the target. 1st column is x
% and 2nd column is y * number of trials
%
% xcursor is the matrix containing position of the cursor (subject). 1st
% column is x and 2nd column is y * number of trials
%
% dyad is boolean, if 1, the trial is dyadic, if 0, solo trial
%
% trial_sequence is the sequence of trials (dyad=1 or solo=0)
%
%
% OUT
%
%
% e is the tracking error (target - cursor)
%
% improv_dual_trial is the improvement after dual trial
%
% improv_single_trial is the improvement after single trial
%
% Mattia D'Alessi
% June 2017


improv_dual_trial_1 = [];
improv_single_trial_1 = [];

improv_dual_trial_2 = [];
improv_single_trial_2 = [];

relative_performance1 = [];

inst_e_1 = abs(xtarget_1 - xcursor_1); % error at every data point
inst_e_2 = abs(xtarget_2 - xcursor_2);
inst_eucl_e_1 = NaN(size(xtarget_1,1), size(xtarget_1,2)/2); %calculate eucliden distance (all time points, 60 columns)
inst_eucl_e_2 = NaN(size(xtarget_2,1), size(xtarget_2,2)/2);
eucl_i = 1;

for ii = 1:2:size(xtarget_1,2)
    inst_eucl_e_1(:,eucl_i) = sqrt(inst_e_1(:,ii).^2 + inst_e_1(:,ii+1).^2);
    inst_eucl_e_2(:,eucl_i) = sqrt(inst_e_2(:,ii).^2 + inst_e_2(:,ii+1).^2);
    eucl_i = eucl_i + 1;
end

e_1 = mean(inst_eucl_e_1); %here it should be a 1*n_trials vector
e_2 = mean(inst_eucl_e_2);

e_1 = e_1(2:end);
e_2 = e_2(2:end);

%% find DS

index_counter = 1;

while index_counter < length(e_1) %keep looking for D trials until trials are over
    
    while not(trial_sequence(index_counter)) % I want to find the dyadic trial
        index_counter = index_counter + 1;
        if index_counter > length(e_1)
            break
        end
    end
    % dual trial found
    index_counter_single = index_counter+1;
    if index_counter > length(e_1)
        break
    end
    
    while (trial_sequence(index_counter_single)) % I want to find the single trial
        index_counter_single = index_counter_single +1;
        if index_counter > length(e_1)
            break
        end
    end
    
    % single trial after dual trial
    % I can compute improvement after dual trial
    improv_dual_trial_1 = [improv_dual_trial_1 e_1(index_counter_single)-e_1(index_counter)];
    improv_dual_trial_2 = [improv_dual_trial_2 e_2(index_counter_single)-e_2(index_counter)];
    
    relative_performance1 = [relative_performance1 e_1(index_counter_single) - e_2(index_counter_single)];
    
    index_counter = index_counter_single + 1;
end

%% find SS

index_counter_single1 = 1;

while (index_counter_single1 < length(e_1)) %keep looking for D trials until trials are over
    
    while (trial_sequence(index_counter_single1)) % I want to find the single trial 1
        index_counter_single1 = index_counter_single1 +1;
    end
    
    % first single trial found
    index_counter_single2 = index_counter_single1 + 1;
    
    
    while (trial_sequence(index_counter_single2)) % I want to find the single trial 2
        index_counter_single2 = index_counter_single2 +1;
    end
    
    if (index_counter_single2 > length(e_1))
        break
    end
    
    % single trial after dual trial
    % I can compute improvement after dual trial
    improv_single_trial_1 = [improv_single_trial_1 e_1(index_counter_single2)-e_1(index_counter_single1)];
    improv_single_trial_2 = [improv_single_trial_2 e_2(index_counter_single2)-e_2(index_counter_single1)];
    
    index_counter_single1 = index_counter_single2;
end

end
