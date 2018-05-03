close all;
clear all;
clc;

% path = './Test 1/';
path = './New Folder/';
listing = dir(path);
for fileNo = 3:size(listing)
    file = strcat(path,listing(fileNo).name);
    fileID = fopen(file,'r');

    data = fread(fileID,19,'*char');
    A = fread(fileID,'uint16','l');
    sizeV = size(A);
    len = fix(sizeV(1)/2);
    % plot (A);
    figure(1)
    clf(1)
%     ylim([1700 2200])
    hold on
     plot (A(1:len-1),'blue');
    plot (A(len+1:len*2-1)*(-1)+4096,'red');
    hold off
    pause(0.5);
    fclose('all')
end
