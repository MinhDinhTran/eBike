function [ rez, minn, maxx ] = ChangeTimeStep( time, data, newinterval )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

    minT = min(time);
    maxT = max(time);
    TimeBase = minT:newinterval:maxT;
    sizey = size(TimeBase);

    arrayRAW = data;
    arrayT = time;
    tim_index = 1;

    sum = 0;
    count = 0;
    rez = zeros(size(TimeBase));
    minn = zeros(size(TimeBase));
    maxx = zeros(size(TimeBase));
    temp = 0;
    lastValue = 0;
    for i = 1:size(arrayT)-1
        if(temp == 0)
            while (tim_index <= sizey(2)  &&  arrayT(i) > TimeBase(tim_index))
                rez(tim_index) = lastValue;
                tim_index = tim_index +1;
            end
            temp = 1;
        end
        if (tim_index <= sizey(2)  &&  arrayT(i) <= TimeBase(tim_index))
            sum = sum + arrayRAW(i);
            if (minn(tim_index) > arrayRAW(i) || minn(tim_index) == 0)
                minn(tim_index) = arrayRAW(i);
            end
            if (maxx(tim_index) < arrayRAW(i))
                maxx(tim_index) = arrayRAW(i);
            end
             if (arrayRAW(i) > 0)
                count = count + 1;
             end
        else
            if (tim_index <= sizey(2))
                temp = 0;
                rez(tim_index) = sum/count;
                if (isnan(rez(tim_index)))
                    rez(tim_index) = 0;%lastValue;
                end;
                lastValue = rez(tim_index);
                tim_index = tim_index +1;
                sum = 0;
                count = 0;
            end
        end
    end
end

