close all;
clear all;
clc;

% path = './Test 1/';
% path = './New Folder/';
% path = './20180508_panemunes tiltas/';
% path = './20180508_2/';


path = './svoriu_test/';
path = './svoriu_test 0-80/';
path = './svoriu_test2 0-100/';
path = './svoriu_test3 0-100/';
path = './pasivazinejimas/';
path = './pasivazinejimas2/';
path = './Stadionas+Deivio kelione i MS/';
path = './20180509_Azuolynas_regeneracija/';

% path = './svoriu_test4 0-100/';
listing = dir(path);
for fileNo = 3:size(listing)
    file = strcat(path,listing(fileNo).name);
    fileID = fopen(file,'r');
    line_ex = fgetl(fileID);
    C = textscan(fileID,'%s %s 	%s	%s	%u');
    fclose('all');
    
    switch listing(fileNo).name
        case 'log_ eBike MCfff4.txt'
            Data.Vbat.Raw = str2double(C{3});
            Data.Vbat.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
        case 'log_ eBike MCfff5.txt'
            Data.Current.Raw = str2double(C{3});
            Data.Current.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
            Data.Current.D = -44.4+Data.Current.Raw*0.0217;
        case 'log_ eBike MCfff6.txt'
            Data.Speed.Raw = str2double(C{3});
            Data.Speed.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
        case 'log_ eBike MCfff8.txt'
            Data.Power.Raw = str2double(C{3});
            Data.Power.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
        case 'log_ eBike Pedal Lffef.txt'
            Data.PedalL.Raw = str2double(C{3});
            Data.PedalL.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
        case 'log_ eBike Pedal Rffef.txt'
            Data.PedalR.Raw = str2double(C{3});
            Data.PedalR.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
        case 'log_CustomServicefff1.txt'
            Data.Mode.Raw = str2double(C{3});
            Data.Mode.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
        case 'log_CustomServicefff2.txt'
            Data.Duty.Raw = str2double(C{3});
            Data.Duty.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
            
        case 'log_ MI Band 22a37.txt'
            Data.HR.Raw = str2double(C{3});
            Data.HR.T = datetime(C{2},'InputFormat','HH:mm:ss.SSS');
            x = find(Data.HR.Raw<0);
            Data.HR.Raw(x) = Data.HR.Raw(x) + 255;
        otherwise
            
    end
end
clearvars -except Data
save('Data')










