%% parameters_base_1fm

%% 'globals'
sampleTime = 0.001;             % sample time [s] Note: make sure the same sample time is set in TwinCAT XAE.
Lf = 0.238;                     % forearm length [m]
Lu = 0.153;                     % upperarm length [m]
Lb = 0.07;                      % base width [m]

%% RobotStruct_FM1
clear FM1

FM1.HomeLocationOpSpace = [0;0.25];                     % home location (homing) [m]
FM1.HomeLocationSize = 0.01;

FM1.DynModParams = [1 1 1 1 1 1 1]';                     % dynamic model parameters (I1,I2,mehat,Fs1,Fv1,Fs2,Fv2)

% actuator 1 data
FM1.Actuator1.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
FM1.Actuator1.AbsEncoderOffset = 56176/FM1.Actuator1.JointAbsoluteEncoderCounts_rev*2*pi + asin(8/153);                     % offset absolute encoder [rad]
FM1.Actuator1.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
FM1.Actuator1.FOAWNoiseLevel = 0.5*2*pi/FM1.Actuator1.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
FM1.Actuator1.transmissionRatio = 7.2976;               % transmission ratio (estimated)[-]
FM1.Actuator1.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
FM1.Actuator1.drivePeakCurrent = 20;                    % drive peak current [A]
FM1.Actuator1.AbsoluteMaximumTorque_Nm = 8;             % maximum allowable torque (at joints) [Nm]
FM1.Actuator1.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
FM1.Actuator1.JointPositionGuardMin = -25*pi/180;       % maximum allowable joint position [rad]
FM1.Actuator1.JointPositionGuardMax = 135*pi/180;       % maximum allowable joint position [rad]

% actuator 2 data
FM1.Actuator2.JointAbsoluteEncoderCounts_rev = 2^16;    % encoder counts per revolution [counts]
FM1.Actuator2.AbsEncoderOffset = 46177/FM1.Actuator2.JointAbsoluteEncoderCounts_rev*2*pi + pi - asin(8/153);                     % offset absolute encoder [rad]
FM1.Actuator2.MotorEncoderCounts_rev = 2^12;            % motor encoder resolution [counts] (1024 lines & X4: 4096 counts)
FM1.Actuator2.FOAWNoiseLevel = 0.5*2*pi/FM1.Actuator2.JointAbsoluteEncoderCounts_rev; % Noise level for FOAW algorithm
FM1.Actuator2.transmissionRatio = 7.3310;               % transmission ratio (estimated) [-]
FM1.Actuator2.motorTorqueConstant = 84.86*1e-3;         % motor torque constant [Nm/A]
FM1.Actuator2.drivePeakCurrent = 20;                    % drive peak current [A]
FM1.Actuator2.AbsoluteMaximumTorque_Nm = 8;             % maximum allowable torque (at joints) [Nm]
FM1.Actuator2.TransmissionSlipAngleGuardMax = 5*pi/180; % maximum allowable transmission slip [Nm]
FM1.Actuator2.JointPositionGuardMin = 45*pi/180;        % maximum allowable joint position [rad]
FM1.Actuator2.JointPositionGuardMax = 205*pi/180;       % maximum allowable joint position [rad]

% force/torque sensor data
FM1.FTSensor.MaxAllowableForcesTorqueSensor = 0.7*[100 100 200 2 2 2]'; % maximum allowable forces and torques
FM1.FTSensor.TransformationMatrixSide = 1; % 1 = transformation matrix based from q1, 2 = transformation from q5.