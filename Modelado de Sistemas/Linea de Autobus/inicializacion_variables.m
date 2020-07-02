%INICIALIZACION DE VARIABLES%
%la linea trabajara desde las 6:00 am hasta las 10:00 pm
h_inicio = 6;
h_final = 22;
t_trabajo = 3600*(h_final - h_inicio)/5; %unidad de tiempos con la que
%trabajajaremos son 5 segundos

%INICIALIZACION DE PARADAS%
%La línea de autobús tendra 36 paradas (las de 657 de MONCLOA-MONTECLARO)
% N=36; -> N ENTRA EN LA LLAMADA AL MODELO
%abrir archivo con el nombre de las paradas
filename = fullfile('lista_paradas.csv');
fileID = fopen(filename);
C = textscan(fileID,'%s');
fclose(fileID);

%La clase parada tiene 4 variables
%guardaremos en un map los clientes en la parada, el id y nombre
llave_paradas = {'ID','NOMBRE','CP'};

%guardaremos en una matriz de tiempos el nº de clientes para cada t
%vector con que inicializar CPT
m_t=[1:t_trabajo;zeros(1,t_trabajo)];
cpt=m_t';



%loop por las 36 paradas

%aprovechamos para contar los clientes que entran en el sistema
n_total_clientes=0;
for p = 1:36
    nombre=strsplit(C{1}{p+1},',');
    nombre=nombre{2};
    M = containers.Map(llave_paradas,{p,nombre,0});
    %valores map
    paradas{1}{p}=M;
    
    %matriz de tiempos
    paradas{2}{p}=cpt;
    
    %TIEMPO ENTRE CLIENTES EXPONENCIAL
    %generaremos 3000 valores de una exponencia (la suma debería dar 36k
    %unidades de tiempo, suficiente para cubrir t_trabajo (12k)
    e_rand= exprnd(1,3000,1);
    
    %los valores de e_rand estan en minutos, los convertiremos a unidad de
    %trabajo
    e_rand= e_rand*12;
    
    %añadiremos un retardo (el tiempo de medio recorrido entre dos paradas)
    %para el primer servicio
    e_rand(1)=e_rand(1)+24*p-1;
    c_sum_e = cumsum(e_rand);
    
    a=1;
    entra_cliente=0;
    actualizar_cliente = zeros(1,t_trabajo);
    for t = 1:t_trabajo
        if (c_sum_e(a) < t)
            entra_cliente=1;
            a=a+1;
        end
        if (p==36)
            entra_cliente=0;
        end
        actualizar_cliente(t)=entra_cliente;
        entra_cliente=0;
    end
    %vector para saber si en un tiempo t entra un cliente en la parada
    paradas{3}{p} = actualizar_cliente;
    n_total_clientes=n_total_clientes+sum(actualizar_cliente);
    
    %id de clientes en la parada
    paradas{4}{p}=[];
end
vars={'C','fileID','filename'};
clear(vars{:});

%INICIALIZACION DE AUTBUSES%

%La clase autobus tiene 5 variables
%el los pasajeros CA, el tiempo de entrada al sistema TI,
%el tiempo de salida TF, la parada a la que se dirige P
%y el ID
llave_autobus = {'ID','CA','P','TI','TF'};

%pasajeros en t - CAT lo modelaremos con una matriz de tiempos
%matriz con la que que inicializar CAT
m_t=[1:t_trabajo;zeros(1,t_trabajo)];
cat=m_t';

%pondremos 200 autobuses en circulación (sabiendo que han podido volver a
%chocheras y salir de nuevo)
for a = 1:200
    M = containers.Map(llave_autobus,{a,0,1,0,t_trabajo});
    autobuses{1}{a}=M;
    autobuses{2}{a}=cat;
    
    %TIEMPO DE RECORRIDOS
    %generaremos 35 valores de gamma para cada autobús a
    g_rand= gamrnd(4,0.5,35,1);
    
    %los valores de g_rand estan en minutos, los convertiremos a unidad de
    %trabajo
    g_rand = g_rand*12;
    
    %redondearemos para facilitar los cálculos
    g_rand = arrayfun(@round,g_rand);
    
    %c_sum_g = cumsum(g_rand);
   
    %cada autobus tiene diferentes tiempos de recorrido
    %el recorrido 1 (desde cocheras hasta la primera parada
    %se tarda 0 segundos y el final de recorrido tm
    autobuses{3}{a}=[0;g_rand;0];
    
    %ids de los pasajeros en el bus
    autobuses{4}{a}=[];
    
    
end

%sale un autbus en la línea en el t=0
a_l_f=0;


%INICIALIZACION DE CLIENTES%

%La clase cliente tiene 5 variables
llave_clientes = {'ID','PI','PF','TI','TF','BP','IDA'};

%funcion de densidad para la bajada de viajeros (y)
n=30;
x=1:n;
y=1:n;
for c = 1:n
 y(c)=0.2*0.8^(c-1);
end

%generamos variables aleatorias para cada cliente
bajad_paradas=gendist(y,1,n_total_clientes);

for c = 1:n_total_clientes
    M = containers.Map(llave_clientes,{c,0,0,0,t_trabajo,bajad_paradas(c),0});
    clientes{1}{c}=M;
end

