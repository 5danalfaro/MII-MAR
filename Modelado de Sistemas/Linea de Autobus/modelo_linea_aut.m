%Inicializar variables
%Número de paradas
%N=36; %Comentar esta linea se se hacen pruebas con N
%Periodo de salida de cocheras
T=5; %Comentar esta linea se se hacen pruebas con T
inicializacion_variables;
%loop por cada unidad de tiempo de trabajo
n_cliente=1;
auto_en_linea=[];
for t = 1:t_trabajo
    %los autobuses salen cada 5 minutos (cada 5*12 unidades de tiempo)
    if(or( t == 1, mod(t,T*12) == 0))
        a_l_f=a_l_f+1;
        auto_en_linea=[auto_en_linea,a_l_f];
        %momento de salida del autobus a_i
        autobuses{1}{a_l_f}('TI')=t;
        %tiempo hasta siguiente parada
        t_next_stop(a_l_f)= autobuses{3}{a_l_f}(1);
    end

   %ENTRADA DE CLIENTES EN LA PARADA
    for p = 1:36
        %si en este tiempo entra un cliente aumenta en uno el número de
        %clientes
        if(paradas{3}{p}(t) == 1)
            %añade un cliente a la parada
            paradas{1}{p}('CP')=paradas{1}{p}('CP')+1;

            %actualiza entrada de este nuevo cliente
            clientes{1}{n_cliente}('TI')=t;
            clientes{1}{n_cliente}('PI')=p;

            %actualiza la parada final de este cliente
            b_c = clientes{1}{n_cliente}('BP');
            if(b_c>(36-p))
                clientes{1}{n_cliente}('BP') = 36-p;
            end

            clientes{1}{n_cliente}('PF')=p+clientes{1}{n_cliente}('BP');

            %añadir id en la parada
            paradas{4}{p} = [n_cliente,paradas{4}{p}];

            %contador de clientes
            n_cliente=n_cliente+1;
        end
         %añadir clientes en tiempo t en la parada
         paradas{2}{p}(t,2)=paradas{1}{p}('CP');
    end

   %LOOP POR TODOS LOS AUTOBUSES DE LA LINEA

    for a_ts = auto_en_linea
        %Si el autobus ha llegado a su ultima parada sacalo de circulación
        if(autobuses{1}{a_ts}('P')> 36)
            autobuses{1}{a_ts}('TF')=t;
            auto_en_linea= auto_en_linea(auto_en_linea~=a_ts);
        end

        if(t_next_stop(a_ts) == 0)
            %BAJADA DE PASAJEROS
            %ids de los pasajeros
            id_ps = autobuses{4}{a_ts};

            %loop por todos los pasajeros
            for p_a_ts = 1:length(id_ps)
                %id de pasajero
                id_p = id_ps(p_a_ts);
                %cuantas paradas quedan para que el cliente se baje
                parada_p = clientes{1}{id_p}('BP');
                %si solo queda 1 o esta en su ultima parada el autobus
                if(or(parada_p == 1, autobuses{1}{a_ts}('P') >= 36))
                    %actualiza el valor del número de clientes en el bus
                    autobuses{1}{a_ts}('CA')=autobuses{1}{a_ts}('CA')-1;
                    %actualiza los ids de pasajeros del bus
                    autobuses{4}{a_ts} = autobuses{4}{a_ts}(autobuses{4}{a_ts}~=id_p);
                end
                clientes{1}{id_p}('BP')=clientes{1}{id_p}('BP')-1;
            end

            %RECOGIDA DE CLIENTES
            %parada en la que recoge los clientes
            par_a = autobuses{1}{a_ts}('P');

            %clientes en la parada
            c_p = paradas{1}{par_a}('CP');

            %ids de clientes en parada
            ids_c_p=paradas{4}{par_a};

            %actualizar el tiempo a la siguiente parada
            %tiempo del siguiente recorrido
            t_next_rec=autobuses{3}{a_ts}(par_a+1);
            %tiempo de espera en la parada 10 + 5N (s) pasado a unidad de
            %trabajo 2+N
            t_esp_parada = 2+c_p;
            t_next_stop(a_ts) = t_esp_parada+t_next_rec;

            %actualizar los pasageros en el autobus
            %numero de pasajeros
            autobuses{1}{a_ts}('CA')=autobuses{1}{a_ts}('CA')+c_p;
            %id de los pasajeros
            autobuses{4}{a_ts}=[ids_c_p,autobuses{4}{a_ts}];

            %actualizar los clientes en la parada
            %en la variable CP
            paradas{1}{par_a}('CP')=0;
            %en la variable CPT
            paradas{2}{p}(t,2)=0;
            for cl_p = 1:length(ids_c_p)
                id_cl = ids_c_p(cl_p);
                clientes{1}{id_cl}('TF') = t;
                clientes{1}{id_cl}('IDA')= a_ts;
            end

            paradas{4}{par_a}=[];


            %actualizar la parada a la que se dirige el autbbus
            autobuses{1}{a_ts}('P') = par_a + 1;
        end

        %actualizar el numero de pasageros en el bus para t - CAT
        autobuses{2}{a_ts}(t,2)=autobuses{1}{a_ts}('CA');

        %actualizar next stop para cada autobus
        t_next_stop(a_ts)=t_next_stop(a_ts)-1;
    end



end

%Generar gráficos de resultados
graficos_de_resultados;
