%% fixtrialnumbers
% In case we had to restart the experiment or twincat (i.e. the data_part
% count is restarted). Combine the data files of two separate folders (from
% folders) into one folder, and keep increasing the counter
%
% Niek Beckers

% absolute paths (or copy this function to the parent folder in question.
% fromfolders = {'session2_1';'session2_2'};
fromfolders = {'part1';'part2'}
tofolder = 'combineddata';

if ~exist(tofolder,'dir')
    mkdir(tofolder);
end

% copy and rename files
cntr = 1;
for ii = 1:length(fromfolders)
    files = dir([fromfolders{ii} filesep '*.mat']);
    filenames = sort_nat({files(:).name});
    for jj = 1:length(filenames)
        copyfile([fromfolders{ii} filesep filenames{jj}],[tofolder filesep 'data_part' num2str(cntr) '.mat']);
        cntr = cntr+1;
    end
    clear files filenames
end