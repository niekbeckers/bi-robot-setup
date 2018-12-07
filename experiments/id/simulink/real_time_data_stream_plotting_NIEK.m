%%RealTime Data Streaming with Delsys SDK

% Copyright (C) 2011 Delsys, Inc.
% 
% Permission is hereby granted, free of charge, to any person obtaining a 
% copy of this software and associated documentation files (the "Software"), 
% to deal in the Software without restriction, including without limitation 
% the rights to use, copy, modify, merge, publish, and distribute the 
% Software, and to permit persons to whom the Software is furnished to do so, 
% subject to the following conditions:
% 
% The above copyright notice and this permission notice shall be included in 
% all copies or substantial portions of the Software.
% 
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
% FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
% DEALINGS IN THE SOFTWARE.

function real_time_data_stream_plotting_NIEK


% CHANGE THIS TO THE IP OF THE COMPUTER RUNNING THE TRIGNO CONTROL UTILITY
% HOST_IP = '192.168.1.60';
HOST_IP = '130.89.64.30';
%%
%This example program communicates with the Delsys SDK to stream 16
%channels of EMG data and 48 channels of ACC data.


%% NIEK's CUSTOM SENSOR PLOT
global partner1sensors;
partner1sensors = [9 1 2 3 4 10 11];

global partner2sensors;
partner2sensors = [12 5 6 7 8 13 14];


%% Create the required objects

%Define number of sensors
NUM_SENSORS = 16;

%handles to all plots
global plotHandlesEMG;
plotHandlesEMG = zeros(NUM_SENSORS,1);

global plotHandlesEMG_BROS1;
plotHandlesEMG_BROS1 = zeros(8,1);
global plotHandlesEMG_BROS2;
plotHandlesEMG_BROS2 = zeros(8,1);

global plotHandlesACC;
plotHandlesACC = zeros(NUM_SENSORS*3, 1);
global rateAdjustedEmgBytesToRead;

%TCPIP Connection to stream EMG Data
interfaceObjectEMG = tcpip(HOST_IP,50041);
interfaceObjectEMG.InputBufferSize = 6400;

%TCPIP Connection to stream ACC Data
interfaceObjectACC = tcpip(HOST_IP,50042);
interfaceObjectACC.InputBufferSize = 6400;

%TCPIP Connection to communicate with SDK, send/receive commands
commObject = tcpip(HOST_IP,50040);

%Timer object for drawing plots.
t = timer('Period', .1, 'ExecutionMode', 'fixedSpacing', 'TimerFcn', {@updatePlots, plotHandlesEMG});
global data_arrayEMG
data_arrayEMG = [];
global data_arrayACC
data_arrayACC = [];

%% Set up the plots


axesHandlesEMG = zeros(NUM_SENSORS,1);

axesHandlesEMG_BROS1 = zeros(8,1);
axesHandlesEMG_BROS2 = zeros(8,1);

axesHandlesACC = zeros(NUM_SENSORS,1);

%initiate the EMG figure
figureHandleEMG = figure('Name', 'EMG Data','Numbertitle', 'off',  'CloseRequestFcn', {@localCloseFigure, interfaceObjectEMG, interfaceObjectACC, commObject, t});
set(figureHandleEMG, 'position', [50 200 750 750])

for i = 1:NUM_SENSORS
    axesHandlesEMG(i) = subplot(4,4,i);

    plotHandlesEMG(i) = plot(axesHandlesEMG(i),0,'-y','LineWidth',1);

    set(axesHandlesEMG(i),'YGrid','on');
    %set(axesHandlesEMG(i),'YColor',[0.9725 0.9725 0.9725]);
    set(axesHandlesEMG(i),'XGrid','on');
    %set(axesHandlesEMG(i),'XColor',[0.9725 0.9725 0.9725]);
    set(axesHandlesEMG(i),'Color',[.15 .15 .15]);
    set(axesHandlesEMG(i),'YLim', [-.005 .005]*0.4);
    set(axesHandlesEMG(i),'YLimMode', 'manual');
    set(axesHandlesEMG(i),'XLim', [0 2000]);
    set(axesHandlesEMG(i),'XLimMode', 'manual');
    
    if(mod(i, 4) == 1)
        ylabel(axesHandlesEMG(i),'V');
    else
        set(axesHandlesEMG(i), 'YTickLabel', '')
    end
    
    if(i >12)
        xlabel(axesHandlesEMG(i),'Samples');
    else
        set(axesHandlesEMG(i), 'XTickLabel', '')
    end
    
    title(sprintf('EMG %i', i)) 
end


% PARTNER 1 figure
figureHandleEMG_BROS1 = figure('Name', 'EMG BROS1','Numbertitle', 'off',  'CloseRequestFcn', {@localCloseFigure, interfaceObjectEMG, interfaceObjectACC, commObject, t});
set(figureHandleEMG_BROS1, 'position', [50 500 750 750])

for i = 1:7
    axesHandlesEMG_BROS1(i) = subplot(2,4,i);

    plotHandlesEMG_BROS1(i) = plot(axesHandlesEMG_BROS1(i),0,'-y','LineWidth',1);

    set(axesHandlesEMG_BROS1(i),'YGrid','on');
    %set(axesHandlesEMG(i),'YColor',[0.9725 0.9725 0.9725]);
    set(axesHandlesEMG_BROS1(i),'XGrid','on');
    %set(axesHandlesEMG(i),'XColor',[0.9725 0.9725 0.9725]);
    set(axesHandlesEMG_BROS1(i),'Color',[.15 .15 .15]);
    set(axesHandlesEMG_BROS1(i),'YLim', [-.005 .005]*0.4);
    set(axesHandlesEMG_BROS1(i),'YLimMode', 'manual');
    set(axesHandlesEMG_BROS1(i),'XLim', [0 2000]);
    set(axesHandlesEMG_BROS1(i),'XLimMode', 'manual');
    
    if(mod(i, 4) == 1)
        ylabel(axesHandlesEMG_BROS1(i),'V');
    else
        set(axesHandlesEMG_BROS1(i), 'YTickLabel', '')
    end
    
    if(i >12)
        xlabel(axesHandlesEMG_BROS1(i),'Samples');
    else
        set(axesHandlesEMG_BROS1(i), 'XTickLabel', '')
    end
    
    title(sprintf('EMG %i', partner1sensors(i))) 
end

% PARTNER 2 figure
figureHandleEMG_BROS2 = figure('Name', 'EMG BROS2','Numbertitle', 'off',  'CloseRequestFcn', {@localCloseFigure, interfaceObjectEMG, interfaceObjectACC, commObject, t});
set(figureHandleEMG_BROS2, 'position', [850 500 750 750])

for i = 1:7
    axesHandlesEMG_BROS2(i) = subplot(2,4,i);

    plotHandlesEMG_BROS2(i) = plot(axesHandlesEMG_BROS2(i),0,'-y','LineWidth',1);

    set(axesHandlesEMG_BROS2(i),'YGrid','on');
    %set(axesHandlesEMG(i),'YColor',[0.9725 0.9725 0.9725]);
    set(axesHandlesEMG_BROS2(i),'XGrid','on');
    %set(axesHandlesEMG(i),'XColor',[0.9725 0.9725 0.9725]);
    set(axesHandlesEMG_BROS2(i),'Color',[.15 .15 .15]);
    set(axesHandlesEMG_BROS2(i),'YLim', [-.005 .005]*0.4);
    set(axesHandlesEMG_BROS2(i),'YLimMode', 'manual');
    set(axesHandlesEMG_BROS2(i),'XLim', [0 2000]);
    set(axesHandlesEMG_BROS2(i),'XLimMode', 'manual');
    
    if(mod(i, 4) == 1)
        ylabel(axesHandlesEMG_BROS2(i),'V');
    else
        set(axesHandlesEMG_BROS2(i), 'YTickLabel', '')
    end
    
    if(i >12)
        xlabel(axesHandlesEMG_BROS2(i),'Samples');
    else
        set(axesHandlesEMG_BROS2(i), 'XTickLabel', '')
    end
    
    title(sprintf('EMG %i', partner2sensors(i))) 
end


%initiate the ACC figure
figureHandleACC = figure('Name', 'ACC Data', 'Numbertitle', 'off', 'CloseRequestFcn', {@localCloseFigure, interfaceObjectEMG, interfaceObjectACC, commObject, t});
set(figureHandleACC, 'position', [850 200 750 750]);

for i= 1:NUM_SENSORS
    axesHandlesACC(i) = subplot(4, 4, i);
    hold on
    plotHandlesACC(i*3-2) = plot(axesHandlesACC(i), 0, '-y', 'LineWidth', 1);    
    plotHandlesACC(i*3-1) = plot(axesHandlesACC(i), 0, '-y', 'LineWidth', 1);   
    plotHandlesACC(i*3) = plot(axesHandlesACC(i), 0, '-y', 'LineWidth', 1);
    hold off 
    
    set(plotHandlesACC(i*3-2), 'Color', 'r')
    set(plotHandlesACC(i*3-1), 'Color', 'b')
    set(plotHandlesACC(i*3), 'Color', 'g')    
    set(axesHandlesACC(i),'YGrid','on');
    %set(axesHandlesACC(i),'YColor',[0.9725 0.9725 0.9725]);
    set(axesHandlesACC(i),'XGrid','on');
    %set(axesHandlesACC(i),'XColor',[0.9725 0.9725 0.9725]);
    set(axesHandlesACC(i),'Color',[.15 .15 .15]);
    set(axesHandlesACC(i),'YLim', [-8 8]);
    set(axesHandlesACC(i),'YLimMode', 'manual');
    set(axesHandlesACC(i),'XLim', [0 2000/13.5]);
    set(axesHandlesACC(i),'XLimMode', 'manual');
    
    if(i > 12)
        xlabel(axesHandlesACC(i),'Samples');
    else
        set(axesHandlesACC(i), 'XTickLabel', '');
    end
    
    if(mod(i, 4) == 1)
        ylabel(axesHandlesACC(i),'g');
    else
        set(axesHandlesACC(i) ,'YTickLabel', '')
    end
    
    title(sprintf('ACC %i', i)) 

end

%%Open the COM interface, determine RATE

fopen(commObject);

pause(1);
fread(commObject,commObject.BytesAvailable);
fprintf(commObject, sprintf(['RATE 2000\r\n\r']));
pause(1);
fread(commObject,commObject.BytesAvailable);
fprintf(commObject, sprintf(['RATE?\r\n\r']));
pause(1);
data = fread(commObject,commObject.BytesAvailable);

emgRate = strtrim(char(data'));
if(strcmp(emgRate, '1925.926'))
    rateAdjustedEmgBytesToRead=1664;
else 
    rateAdjustedEmgBytesToRead=1728;
end


%% Setup interface object to read chunks of data
% Define a callback function to be executed when desired number of bytes
% are available in the input buffer
 bytesToReadEMG = rateAdjustedEmgBytesToRead;
 interfaceObjectEMG.BytesAvailableFcn = {@localReadAndPlotMultiplexedEMG,plotHandlesEMG,bytesToReadEMG};
 interfaceObjectEMG.BytesAvailableFcnMode = 'byte';
 interfaceObjectEMG.BytesAvailableFcnCount = bytesToReadEMG;
 
 bytesToReadACC = 384;
interfaceObjectACC.BytesAvailableFcn = {@localReadAnPlotMultiplexedACC, plotHandlesACC, bytesToReadACC};
interfaceObjectACC.BytesAvailableFcnMode = 'byte';
interfaceObjectACC.BytesAvailableFcnCount = bytesToReadACC;

drawnow
start(t);

%pause(1);
%% 
% Open the interface object
try
    fopen(interfaceObjectEMG);
    fopen(interfaceObjectACC);
catch
    
    localCloseFigure(figureHandleACC,1 ,interfaceObjectACC, interfaceObjectEMG, commObject, t);
    delete(figureHandleEMG);
    delete(figureHandleEMG_BROS1);
    delete(figureHandleEMG_BROS2);
    error('CONNECTION ERROR: Please start the Delsys Trigno Control Application and try again');
end



%%
% Send the commands to start data streaming
fprintf(commObject, sprintf(['START\r\n\r']));


%%
% Display the plot

%snapnow;


%% Implement the bytes available callback
%The localReadandPlotMultiplexed functions check the input buffers for the
%amount of available data, mod this amount to be a suitable multiple.

%Because of differences in sampling frequency between EMG and ACC data, the
%ratio of EMG samples to ACC samples is 13.5:1

%We use a ratio of 27:2 in order to keep a whole number of samples.  
%The EMG buffer is read in numbers of bytes that are divisible by 1728 by the
%formula (27 samples)*(4 bytes/sample)*(16 channels)
%The ACC buffer is read in numbers of bytes that are divisible by 384 by
%the formula (2 samples)*(4 bytes/sample)*(48 channels)
%Reading data in these amounts ensures that full packets are read.  The 
%size limits on the dataArray buffers is to ensure that there is always one second of
%data for all 16 sensors (EMG and ACC) in the dataArray buffers
function localReadAndPlotMultiplexedEMG(interfaceObjectEMG, ~,~,~, ~)
global rateAdjustedEmgBytesToRead;
bytesReady = interfaceObjectEMG.BytesAvailable;
bytesReady = bytesReady - mod(bytesReady, rateAdjustedEmgBytesToRead);%%1664

if (bytesReady == 0)
    return
end
global data_arrayEMG
data = cast(fread(interfaceObjectEMG,bytesReady), 'uint8');
data = typecast(data, 'single');




if(size(data_arrayEMG, 1) < rateAdjustedEmgBytesToRead*19)
    data_arrayEMG = [data_arrayEMG; data];
else
    data_arrayEMG = [data_arrayEMG(size(data,1) + 1:size(data_arrayEMG, 1));data];
end

function localReadAnPlotMultiplexedACC(interfaceObjectACC, ~, ~, ~, ~)

bytesReady = interfaceObjectACC.BytesAvailable;
bytesReady = bytesReady - mod(bytesReady, 384);

if(bytesReady == 0)
    return
end
global data_arrayACC
data = cast(fread(interfaceObjectACC, bytesReady), 'uint8');
data = typecast(data, 'single');





if(size(data_arrayACC, 1) < 7296)
    data_arrayACC = [data_arrayACC; data];
else
    data_arrayACC = [data_arrayACC(size(data, 1) + 1:size(data_arrayACC, 1)); data];
end


%% Update the plots
%This timer callback function is called on every tick of the timer t.  It
%demuxes the dataArray buffers and assigns that channel to its respective
%plot.
function updatePlots(obj, Event,  tmp)
global data_arrayEMG
global plotHandlesEMG
for i = 1:size(plotHandlesEMG, 1) 
    data_ch = data_arrayEMG(i:16:end);      
    set(plotHandlesEMG(i), 'Ydata', data_ch)
end

% partner 1
global plotHandlesEMG_BROS1;
global partner1sensors;

for i = 1:size(plotHandlesEMG_BROS1,1)
    idx = partner1sensors(i);
    data_ch = data_arrayEMG(idx:16:end);
    set(plotHandlesEMG_BROS1(i), 'YData', data_ch);
end

% partner 2
global plotHandlesEMG_BROS2;
global partner2sensors;

for i = 1:size(plotHandlesEMG_BROS2,1)
    idx = partner2sensors(i);
    data_ch = data_arrayEMG(idx:16:end);
    set(plotHandlesEMG_BROS2(i), 'YData', data_ch);
end

global data_arrayACC
global plotHandlesACC
for i = 1:size(plotHandlesACC, 1)
    data_ch = data_arrayACC(i:48:end);
    set(plotHandlesACC(i), 'Ydata', data_ch)
end
drawnow
    


%% Implement the close figure callback
%This function is called whenever either figure is closed in order to close
%off all open connections.  It will close the EMG interface, ACC interface,
%commands interface, and timer object
function localCloseFigure(figureHandle,~,interfaceObject1, interfaceObject2, commObject, t)

%% 
% Clean up the network objects
if isvalid(interfaceObject1)
    fclose(interfaceObject1);
    delete(interfaceObject1);
    clear interfaceObject1;
end
if isvalid(interfaceObject2)
    fclose(interfaceObject2);
    delete(interfaceObject2);
    clear interfaceObject2;
end



if isvalid(t)
   stop(t);
   delete(t);
end

if isvalid(commObject)
    fclose(commObject);
    delete(commObject);
    clear commObject;
end

%% 
% Close the figure window
delete(figureHandle);
