function s=DecodeBROSError(x)
% This function returns a multiline string-array explaining, in understandable text, the
% DriveError values.
% The input argument x can be either:
% <number>: This number is decoded. 
% <block id> (as obtained by `gcb`): The data from this display block is used
% [<number>,<number>]: Assumes that these TWO numbers
%          represent erorr codes of BROS1 and BROS2
%% Get the DriveError values
if isscalar(x)
    valuesToDecode = x;
    brosNames = {'BROS'};
elseif ischar(x) % block identifier
    rto = get_param(x, 'RunTimeObject');
    if isempty(rto)
        s = 'Error: no data in block (is the model running and connected?)';
        return 
    end;
    p = get(rto.InputPort(1));
    valuesToDecode = p.Data;
    brosNames = {'BROS1','BROS2'};
elseif isvector(x) 
    if numel(x)~=2
        s='Error: vector should have 2 elements';
    end;
    valuesToDecode = x;
    brosNames = {'BROS1','BROS2'};
else
    s = 'Error: Unknown format of argument passed to DecodeDriveError';
    return
end;

%% Decode the value
errorSingleActuator = {...
    'Joint position guard',...
    'Joint velocity guard',...
    'Joint torque guard',...
    'Transmission slip',...
    'Drive error',...
    'Emergency button pressed', ...
    'Motor undervoltage', ...
    'Motor overvoltage', ...
    'Drive overtemperature', ...
    'I2t error', ...
    'Overcurrent', ...
    'Following/control error', ...
    'Short circuit error',...
    'Other drive error (see MotionErrorRegister)'
    };
errorDescriptions = [...
    strcat('Act1: ',errorSingleActuator);
    strcat('Act2: ',errorSingleActuator);
    'Endpoint position guard',...
    'High condition number (singular)',...
    'Internal angle guard',...
    'F/T sensor range exceeded'
   ];
s='';
for i=1:length(valuesToDecode)
    v = valuesToDecode(i);
    n = brosNames{i};
    
    if valuesToDecode(i)<=-1
        inUseStr='(not in use)';
    else
        inUseStr='';
    end;
    s=strvcat(s,sprintf('%s: %s %s',brosNames{i},hex(valuesToDecode(i)),inUseStr));
    anyError = 0;
    
    for j = 1:length(errorDescriptions)
        vv = int32(abs(v));
        n = int32(2^(j-1));
        if bitand(vv,n)
            s=strvcat(s,sprintf('   %s %s', hex(n), errorDescriptions{j}));
            anyError = 1;
        end;
    end;
    
    s=strvcat(s,'');
    
end;


function s=hex(x)
% Custom hex function which allows negative numbers and assumes it is an
% int32
if x<0
    x=2^32+x;
end;
s=['0x' dec2hex(x,8)];
