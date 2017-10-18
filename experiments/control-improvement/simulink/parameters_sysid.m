%% parameters_sysid

%% 'globals'
sampleTime = 0.001;             % sample time [s] Note: make sure the same sample time is set in TwinCAT XAE.
fs = 1/sampleTime;              % sampling frequency
fn = fs/2;                      % nyquist frequency
Lf = 0.238;                     % forearm length [m]
Lu = 0.153;                     % upperarm length [m]
Lb = 0.07;                      % base width [m]

load('data_inputsignal.mat');

% butterworth filter (filtering velocity signal) 
fc = 60;
[Bbutter,Abutter] = butter(2, fc/fn);

%% RobotStruct_FM1
% clear FM1

BROS3.HomeLocationOpSpace = [0;0.25];                     % home location (homing) [m]
BROS3.HomeLocationSize = 0.005;
BROS3.NominalPositionOpSpace = [0;0.25];

% FM1.DynModParams = [1.597109e-03  1.580786e-03  7.028260e-02  7.988736e-02 0*-9.954503e-04  1.006366e-01 0*-1.994173e-03]';                     % dynamic model parameters (I1,I2,mehat,Fs1,Fv1,Fs2,Fv2)
BROS3.DynModParams = [9.446018e-04  5.536255e-04  1.102026e-01  1.175426e-01 -6.234047e-03  1.037213e-01  3.853734e-03]';
% actuator 1 data
BROS3.Actuator1.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
BROS3.Actuator1.AbsEncoderOffset = 41228/BROS3.Actuator1.JointAbsoluteEncoderCounts_rev*2*pi + asin(8/153);                     % offset absolute encoder [rad]
BROS3.Actuator1.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
% FM1.Actuator1.FOAWNoiseLevel = 4*2*pi/FM1.Actuator1.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
BROS3.Actuator1.FOAWNoiseLevel = 9e-4;                  % Noise level for FOAW algorithm
BROS3.Actuator1.transmissionRatio = 7.2976;               % transmission ratio (estimated)[-]
BROS3.Actuator1.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
BROS3.Actuator1.drivePeakCurrent = 20;                    % drive peak current [A]
BROS3.Actuator1.AbsoluteMaximumTorque_Nm = 8;             % maximum allowable torque (at joints) [Nm]
BROS3.Actuator1.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
BROS3.Actuator1.JointPositionGuardMin = -25*pi/180;       % maximum allowable joint position [rad]
BROS3.Actuator1.JointPositionGuardMax = 135*pi/180;       % maximum allowable joint position [rad]
BROS3.Actuator1.FilterButterA = Abutter;
BROS3.Actuator1.FilterButterB = Bbutter;

% actuator 2 data
BROS3.Actuator2.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
BROS3.Actuator2.AbsEncoderOffset = 32202/BROS3.Actuator2.JointAbsoluteEncoderCounts_rev*2*pi + pi - asin(8/153);                     % offset absolute encoder [rad]
BROS3.Actuator2.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
% FM1.Actuator2.FOAWNoiseLevel = 4*2*pi/FM1.Actuator2.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
BROS3.Actuator2.FOAWNoiseLevel = 9e-4; % Noise level for FOAW algorithm
BROS3.Actuator2.transmissionRatio = 7.3310;               % transmission ratio (estimated) [-]
BROS3.Actuator2.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
BROS3.Actuator2.drivePeakCurrent = 20;                    % drive peak current [A]
BROS3.Actuator2.AbsoluteMaximumTorque_Nm = 8;             % maximum allowable torque (at joints) [Nm]
BROS3.Actuator2.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
BROS3.Actuator2.JointPositionGuardMin = 45*pi/180;        % maximum allowable joint position [rad]
BROS3.Actuator2.JointPositionGuardMax = 205*pi/180;       % maximum allowable joint position [rad]
BROS3.Actuator2.FilterButterA = Abutter;
BROS3.Actuator2.FilterButterB = Bbutter;

% force/torque sensor data
BROS3.FTSensor.MaxAllowableForcesTorqueSensor = 0.7*[100 100 200 2 2 2]'; % maximum allowable forces and torques
BROS3.FTSensor.TransformationMatrixSide = 1; % 1 = transformation matrix based from q1, 2 = transformation from q5.