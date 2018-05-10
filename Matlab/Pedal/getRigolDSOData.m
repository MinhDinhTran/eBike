function [X, Y] = getRigolDSOData(ScopeHandler, Channel, Progress)
%   [t, Y] = getRigolDSOData(ScopeHandler, Channel, Progress);
%   getRigolDSOData gets data from the Rigol oscilloscopes
%
%   ScopeHandler - Handler to the VISA scope handler
%   Channel = 'CHAN1', 'CHAN2', 'CHAN3', 'CHAN4', 'MATH'
%
%   Progress:   integer number
%               <0  - progress bar shows data transfer progress
%               N   - progress is visualized in a figure(N), where (N>0)
%               0   - no progress indication
%   Example 1:
%       [t, Y] = getRigolDSOData(H_SCOPE, 'CHAN1',2); % gets data from the
%       cahnnel 1, and visualizes the progress in figure 2
%
%   Example 2:
%       [t, Y] = getRigolDSOData(H_SCOPE, 'CHAN1',-1); % gets data from the
%       cahnnel 1, and shows progress in a progress bar
%
%------------------------------------------------------------------------------
narginchk(2,3);

if nargin < 3
    Progress=0; % no progress if 3rd parameters is missing
end

p1=0.01;
TMC_L=11; % TMC header length
fprintf(ScopeHandler, ':WAV:SOUR %s', Channel);
%fprintf(ScopeHandler, ':WAV:MODE %s', Mode); % NORM- just screen data, RAW or MAX- all or just screen, depending on RUN/ STOP state
% fprintf(ScopeHandler, ':WAV:MODE RAW');
fprintf(ScopeHandler, ':WAV:MODE NORM');
% fprintf(ScopeHandler, ':WAV:MODE MAX'); % read RAW in STOP mode, SCREEN points in RUN mode
fprintf(ScopeHandler, ':WAV:FORM BYTE');

START=1; % start memory address
X=[]; % time array
Y=[]; % data array

FRAME=1; % frame number

Frame_Length=250000;
LEN=Frame_Length+TMC_L;
% get preamble data and decode it
fprintf(ScopeHandler, ':WAV:PRE?'); C=fscanf(ScopeHandler);
A=sscanf(C,'%u,%u,%u,%u,%f,%f,%f,%f,%f,%f');
if length(A)==10
    Points=A(3);
    fprintf(ScopeHandler, ':WAV:YREF?'); Yreference=str2num(fscanf(ScopeHandler));
    fprintf(ScopeHandler, ':WAV:YOR?');  Yorigin=str2num(fscanf(ScopeHandler));
    fprintf(ScopeHandler, ':WAV:YINC?'); Yincrement=str2num(fscanf(ScopeHandler));
    
    
    fprintf(ScopeHandler, ':WAV:XREF?'); Xreference=str2num(fscanf(ScopeHandler));
    fprintf(ScopeHandler, ':WAV:XOR?');  Xorigin=str2num(fscanf(ScopeHandler));
    fprintf(ScopeHandler, ':WAV:XINC?'); Xincrement=str2num(fscanf(ScopeHandler));
%   fprintf(ScopeHandler, ':CHAN1:SCALe?'); UNIT=fscanf(ScopeHandler);
% 
%     Xincrement=A(5);   Xorigin=A(6); Xreference=A(7);
%     Yincrement=A(8);   Yorigin=A(9); Yreference=A(10);
%     if Frame_Length>=Points
        Frame_Length=Points
%     end
    N1=1;
    K=0;
    if Frame_Length<Points
        K=Points/Frame_Length;
        N1=floor(K);
    end
    
    START=1;
    
    LEN=Frame_Length+TMC_L;
    FRAME=1;
    %Y=[];
    %Y=zeros(Points,1); % prealocate array
    %while (LEN==(Frame_Length+TMC_L))
    if Progress<0
        h = waitbar(0,'Reading data...');
    end;
    
    for I=1:N1
        
        STOP=START+Frame_Length-1;
        fprintf(ScopeHandler, ':WAV:STAR %u',START);   fprintf(ScopeHandler, ':WAV:STOP %u', STOP);
        %fprintf(ScopeHandler, ':WAV:STOP?'); STOP2=fscanf(ScopeHandler); STOP2=str2num(STOP2);
        
        fprintf(ScopeHandler, ':WAV:DATA?');
        pause(p1);
        [data,LEN]= fread(ScopeHandler, Frame_Length+TMC_L+1);
        %[data,LEN]= fread(ScopeHandler);
        
        N=length(data);
        Y1=data((TMC_L+1):(N-1)); %remove header (11 bytes) and 0x0A at th end
        Y=[Y; Y1];
        %Y(START:STOP)=Y1;
        
        
%         if Progress>0
%             
%             figure(Progress);        plot(Y1);
% %             set(gca,'YLim',[0 255]);
%             title(sprintf('FRAME=%u/%u MEMORY=%u...%u',[FRAME,N1,START,STOP]));
%             xlabel('Samples'); ylabel('RAW ADC');
%             drawnow;
%             
%         end;
        
        if Progress<0
            waitbar(I /N1);
        end;
        START=STOP+1;
        FRAME=FRAME+1;
    end;
    if Progress<0
        close(h);
    end;
    
    
    % get the last frame
    if K>1
        Frame_Length=Points-N1*Frame_Length;
        STOP=START+Frame_Length-1;
        fprintf(ScopeHandler, ':WAV:STAR %u',START);    fprintf(ScopeHandler, ':WAV:STOP %u', STOP);
        fprintf(ScopeHandler, ':WAV:STOP?'); STOP2=fscanf(ScopeHandler); STOP2=str2num(STOP2);
        
        fprintf(ScopeHandler, ':WAV:DATA?');
        pause(p1);
        [data,LEN]= fread(ScopeHandler, Frame_Length+TMC_L+1);
        
        N=length(data);
        Y1=data((TMC_L+1):(N-1)); %remove header (11 bytes) and 0x0A at th end
        Y=[Y; Y1];
    %    Y(START:STOP)=Y1;
        
    end;
    
    % Yincrement=A(8);  Yorigin =A(9); Yreference=A(10);
    Y=(Y-Yreference-Yorigin)*Yincrement;
    
    N=length(Y);
    X=0:N-1; X=X*Xincrement; X=X+Xorigin;
    X=X';
end



end