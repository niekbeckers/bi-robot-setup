function matlabVirtualPartner
callerID = '[MATLABVIRTUALPARTNER]: ';
disp([callerID 'Starting up ' mfilename]);

%% initialize
cntr_filename = 0;
folderpath = 'C:\Users\Labuser\Documents\repositories\bros_experiments\main-projects\ofAppTCControl\ofAppTCControl\matlab\';
filepath = 'vpFitSettings_trial';
loopPause = 0.5;

% parpool
if (size(gcp) == 0)
    parpool(2,'IdleTimeout',30); % setup workers with idle timeout of 30 minutes
end

keepRunning = true;
while (keepRunning)
    try
        [loadOkay,s] = readXML([folderpath filepath num2str(cntr_filename) '.xml']);
    catch
        loadOkay = false;
    end
    
    if loadOkay
        if isfield(s.VP, 'terminate')
            keepRunning = false;
            continue; % go to next iteration of while loop, skip everything below
        end
        errorFlag = 0;
        
        disp([callerID 'Loaded ' filepath num2str(cntr_filename) ', starting model fit']);
        
        % perform optimization
        parfor ii = 1:10
            j = ii;
        end
        
        % DEBUG dummy output
        out.VP.trialID = s.VP.trialID;
        out.VP.executeVirtualPartner.id0 = 0;
        out.VP.executeVirtualPartner.id1 = 0;
        out.VP.error.id0 = 0;
        out.VP.error.id1 = 0;
        out.VP.modelparameters.bros1.x1 = randn(1);
        out.VP.modelparameters.bros1.x2 = randn(1);
        out.VP.modelparameters.bros1.x3 = randn(1);
        out.VP.modelparameters.bros2.x1 = randn(1);
        out.VP.modelparameters.bros2.x2 = randn(1);
        out.VP.modelparameters.bros2.x3 = randn(1);

        % write results to XML file
        
        if (errorFlag == 0)
            outputfile = [folderpath 'fitResults_trial' num2str(out.VP.trialID) '.xml'];
            writeXML(out,outputfile);
            disp([callerID 'Results written to ''fitResults_trial' num2str(out.VP.trialID) '.xml''']);
        end
        cntr_filename = cntr_filename+1;
    end
    pause(loopPause);
end

end

function [loadOkay,s] = readXML(filename)

loadOkay = false;
s = struct;

if exist(filename,'file')
    try
        xml = xml2struct(filename);
        loadOkay = true;
    catch
        loadOkay = false;
        return;
    end
    
    % check if an external terminate request is made.
    if isfield(xml.VP,'terminate')
        s.VP.terminate = true;
        return
    end
    
    % parse
    if isfield(xml.VP, 'trialID')
        s.VP.trialID = str2double(xml.VP.trialID.Text);
    else
        s.VP.trialID = [];
        loadOkay = false;
    end
    
    if isfield(xml.VP, 'doFitForBROSID')
        flds = fieldnames(xml.VP.doFitForBROSID);
        for ii = 1:length(flds)
            s.VP.doFitForBROSID(ii) = str2double(xml.VP.doFitForBROSID.(flds{ii}).Text);
        end
    else
        s.VP.doFitForBROSID = [];
        loadOkay = false;
    end
    
    if isfield(xml.VP,'useX0')
        flds = fieldnames(xml.VP.useX0);
        for ii = 1:length(flds)
            s.VP.useX0(ii) = str2double(xml.VP.useX0.(flds{ii}).Text);
        end
    else
        xml.VP.useX0 = [];
    end
    
end
end

function [writeOkay] = writeXML(s, filename)

try 
    struct2xml(s,filename);
catch
    writeOkay = false;
end

end