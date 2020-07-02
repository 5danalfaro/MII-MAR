function [Ts,dificultad,Kp] = init_var
    
    dif = input('Enter a number for dificulty (1-4)\n');
    if isempty(dif);
        dif=1;
    end
    % ASIGNACIÓN DE PUNTOS DE REFERENCIA
    switch dif
        case 1
            Kp=1.8;
        case 2
            Kp=2.5;
        case 3
            Kp=2;
        case 4
            Kp=1.8;
        otherwise
            Kp=1.8;
    end
    assignin('base','Ts',0.033);
    assignin('base','dificultad',dif);
    assignin('base','Kp',Kp);
end
