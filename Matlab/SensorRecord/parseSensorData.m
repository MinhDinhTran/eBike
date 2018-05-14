close all;
clear all;
clc;

path = './2018-05-13_19-03-38/';
path = './2018-05-13_20-12-44/';

listing = dir(path);
for fileNo = 3:size(listing)
    file = strcat(path,listing(fileNo).name);
    fileID = fopen(file,'r');
    line_ex = fgetl(fileID);
    s = dir(file);
    if s.bytes ~= 0
        M.(erase(listing(fileNo).name,'.csv')) = csvread(file,1,0);
    end
    fclose('all');
end
%     'Accelerometer'
%     'AccelerometerLinear'
%     'AmbientTemperature'
%     'Compass'
%     'Gravity'
%     'Gyroscope'
%     'Light'
%     'Pressure'
%     'RotationVector'

% plot(M.Accelerometer(:,1), M.Accelerometer(:,2),...
%     M.Accelerometer(:,1), M.Accelerometer(:,3),...
%     M.Accelerometer(:,1), M.Accelerometer(:,4));

s = size(M.Gyroscope);
sum = zeros(s(2));
SUM = [];
for i = 1:s(1)
    for j = 2 : s(2)
        sum(j) = sum(j) + M.Gyroscope(i,j);
        SUM(i,j) = sum(j);
    end
end

% 3
plot(M.Gyroscope(:,1), M.Gyroscope(:,2),...
M.Gyroscope(:,1), M.Gyroscope(:,3),...
M.Gyroscope(:,1), M.Gyroscope(:,4))

plot(M.Gyroscope(:,1), SUM(:,2),...
M.Gyroscope(:,1), SUM(:,3),...
M.Gyroscope(:,1), SUM(:,4))

% figure(5)
% plot(SUM(:,2),SUM(:,3),SUM(:,4))

