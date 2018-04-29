close all;
clear all;
clc;

% path = './Test 1/';
path = './New Folder/';

for fileNo = 1:1000
    file = strcat(path,'FILE_',int2str(fileNo),'.txt');
    fileID = fopen(file,'r');

    data = fread(fileID,19,'*char');
    A = fread(fileID,'uint16','l');
    sizeV = size(A);
    len = fix(sizeV(1)/2);
    % plot (A);
    figure(1)
    clf(1)
    hold on
%     plot (A(1:len-1),'blue');
    plot (A(len+1:len*2-1),'red');
    hold off
    pause(0.1);
    fclose('all')
end
