#!/usr/bin/env python3

##########################################################################
# ISCTE-IUL: 1.º - Trabalho prático de Sistemas Operativos 2023/2024
#
# Avaliação automática de 1 trabalho
#
# Versão 0.1
##########################################################################

import subprocess
import os
import sys
import argparse
import datetime
import re
import glob
# import pandas as pd

os.environ["SO_HIDE_COLOURS"] = "1"
stoponerror = False
##########################################################################
# Test data
#
##########################################################################
passageiros = \
"fabio:225608358:Fábio Cardoso:fabio.cardoso@iscteflight.pt:34erdfcv:15000", \
"dlsgl:275067109:David Gabriel:david.gabriel@iscteflight.pt:12qwaszx:10000", \
"nunogarrido:213654377:Nuno Garrido:nuno.garrido@iscteflight.pt:09polkmn:2000"

voos = \
"FR2101:Lisboa:Dubai:2024-05-23:08h00:320:210:35", \
"FR5102:Dubai:Bangkok:2024-03-11:12h30:450:220:220", \
"PR3201:Londres:Nova York:2024-03-09:10h00:1500:285:0", \
"PT6202:Tóquio:Singapura:2024-03-10:16h45:1700:290:10", \
"FR2301:Dubai:Sydney:2024-11-04:17h30:2800:315:150", \
"PT5000:Sydney:Los Angeles:2024-03-11:22h00:3000:520:0", \
"PT5030:Lisboa:Terceira:2024-03-12:22h00:340:175:15"

relatorio = \
"12087:FR5102:Dubai:Bangkok:450:fabio:2024-02-15:12h30", \
"12088:PT5030:Lisboa:Terceira:340:fabio:2024-02-18:09h12", \
"12089:PT6202:Tóquio:Singapura:1700:dlsgl:2024-02-19:22h37", \
"12090:FR5102:Dubai:Bangkok:450:nunogarrido:2024-02-25:08h48", \
"12091:FR5102:Dubai:Bangkok:450:dlsgl:2024-02-25:12h30", \
"12092:PT5000:Sydney:Los Angeles:3000:fabio:2024-02-26:00h30"

##########################################################################
# List of questions
#
###########################################################################
enunciado = {
    "S1.1.1a":"Falha com 0 argumentos",
    "S1.1.1b":"Falha com 1 argumentos",
    "S1.1.1c":"Falha com 2 argumentos",
    "S1.1.1d":"Falha com 5 argumentos",
    "S1.1.2a":"Falha com passageiro inválido",
    "S1.1.2b":"Falha com passageiro inválido",
    "S1.1.2c":"Falha com passageiro inválido",
    "S1.1.3":"Falha com saldo inválido",
    "S1.1.4":"Falha com NIF inválido",
    "S1.2.1":"Detecta 'passageiros.txt' em falta",
    "S1.2.2":"Cria 'passageiros.txt' se necessário",
    "S1.2.3":"Detecta novo passageiro",
    "S1.2.4":"Falha com novo passageiro sem NIF",
    "S1.2.5":"Gera novo ID",
    "S1.2.6":"Gera email",
    "S1.2.7":"Acrescenta novo passageiro",
    "S1.2.7b":"Ficheiro correcto",
    "S1.3.1":"Falha com senha errada",
    "S1.3.2a":"Calcula novo saldo",
    "S1.3.2b":"Actualiza 'passageiros.txt' correctamente",
    "S1.4.1":"Cria ficheiro 'saldos-ordenados.txt'",

    "S2.1.1a":"Verifica se voos.txt existe",
    "S2.1.1b":"Verifica se passageiros.txt existe",
    "S2.1.2a":"Falha com cidade sem voos",
    "S2.1.2b":"Falha com cidade sem voos disponíveis",
    "S2.1.3a":"Menu correcto",
    "S2.1.3b":"Menu falha com opção inválida",
    "S2.1.4":"Falha com utilizador inválido",
    "S2.1.5":"Falha com senha inválida",
    "S2.2.1":"Falha por falta de saldo",
    "S2.2.2a":"Actualiza saldo correctamente",
    "S2.2.2b":"Actualiza 'passageiros.txt' correctamente",
    "S2.2.3a":"Actualiza disponibilidade correctamente",
    "S2.2.3b":"Actualiza ficheiro 'voos.txt' correctamente",
    "S2.2.4a":"Realiza reserva",
    "S2.2.4b":"Actualiza ficheiro 'relatorio_reservas.txt' correctamente",

    "S3.1.1":"Verifica se voos.txt existe",
    "S3.1.2a":"Falha com n.º voo invalido",
    "S3.1.2b":"Falha com partida invalida",
    "S3.1.2c":"Falha com destino invalido",
    "S3.1.2d":"Falha com data invalida",
    "S3.1.2e":"Falha com hora invalida",
    "S3.1.2f":"Falha com preco invalido",
    "S3.1.2g":"Falha com capacidade invalida",
    "S3.1.2h":"Falha com disponibilidade invalida",
    "S3.2.1a":"Cria página html",
    "S3.2.1b":"Página html correcta",

    "S4.1.1a":"Falha com 0 argumentos",
    "S4.1.1b":"Falha com argumento inválido",
    "S4.1.1c":"Falha com opção top sem parâmetro adicional",
    "S4.2.1a":"Opção passageiros correcta (success)",
    "S4.2.1b":"Opção passageiros correcta (stats.txt)",
    "S4.2.2a":"Opção top correcta (success)",
    "S4.2.2b":"Opção top correcta (stats.txt)",

    "S5.1a":"Sai do menu com 0",
    "S5.1b":"Menu correcto",
    "S5.2.2.1":"Chama regista_passageiro.sh correctamente",
    "S5.2.2.2":"Chama compra_bilhete.sh correctamente",
    "S5.2.2.3":"Chama estado_voos.sh correctamente",
    "S5.2.2.4":"Chama Estatísticas/Passageiros correctamente",
    "S5.2.2.5":"Chama Estatisticas/Top correctamente",
    "S5.4":"Chama refill.sh correctamente",
    "S5.5.1":"Chama stats.sh listar corretamente",
    "S5.5.2":"Chama stats.sh popular corretamente",
    "S5.5.3":"Chama stats.sh histograma corretamente",
}

grade = { }

userErrorLog = []

##########################################################################
# Utility functions
#
###########################################################################

if ( sys.stdout.encoding == "UTF-8" or sys.stdout.encoding == "utf-8" ):
  ok   = u"\u001b[32m✔\u001b[0m"
  fail = u"\u001b[31m✗\u001b[0m"
  warn = u"\u001b[33m⚠️\u001b[0m"
else:
  ok = "\u001b[32m[ ok ]\u001b[0m"
  fail = "\u001b[31m[fail]\u001b[0m"
  warn = "\u001b[33m[warn]\u001b[0m"

def create_file( filename, text ):
    """
    Creates a test file from a list of strings

    Parameters
    ----------
    filename : str
        Name of file to write, will be overriten
    text : list of str
        List of strings to write
    """
    f = open( filename, 'w' )
    for line in text:
        print( line, file = f )
    f.close()

def compare_files( fileA, fileB, skip_blanks = True, verbose = False, trim = True, respectOrder = True ):
    """
    Compares two text files, optionally skippping blank lines

    Parameters
    ----------
    fileA : str
        Filename of file A
    fileB : str
        Filename of file B
    skip_blanks : logical [optional]
        If set to True (default) blank lines will be skipped in the comparison
    trim : logical [optional]
        If set to True (default) lines will be trimmed (remove leading and
        trailing blanks) before comparison
    respectOrder : logical [optional]
        If set to True (default) lines will be compared in the same order on both files
        If set to false, the comparison is performed checking if all lines of fileA exist
        in fileB (in any order) and if all lines of fileB exist in fileA (in any order)
    verbose : logical [optional]
        If set to True prints out all different lines between the files. The
        default is not to print different lines.

    Returns
    -------
    compare_files : logical
        Returns true if the two files match, false otherwise
    """
    if ( not os.path.exists(fileA) ):
        print("(*error*) compare_files: file A '{}' not found".format(fileA) )
        return False

    if ( not os.path.exists(fileB) ):
        print("(*error*) compare_files: file B '{}' not found".format(fileB) )
        return False

    with open( fileA, 'r' ) as f:
        dataA = f.read().splitlines()

    with open( fileB, 'r' ) as f:
        dataB =  f.read().splitlines()

    if ( trim ):
        dataA = [ s.strip() for s in dataA ]
        dataB = [ s.strip() for s in dataB ]

    if ( skip_blanks ):
        dataA = [ s for s in dataA if s != '' ]
        dataB = [ s for s in dataB if s != '' ]

    equalContent = False
    equalNrLines = ( len(dataA) == len(dataB) )
    if equalNrLines:
        if ( respectOrder ):
            equalContent = ( dataA == dataB )
            if ( not equalContent and verbose ):
                i = 0
                while ( (i < len(dataA)) or (i < len(dataB) ) ):
                    if ( (i < len(dataA) ) and (i < len(dataB)) ):
                        if ( dataA[i] != dataB[i] ):
                            print('\n{:>30}:{} - {}'.format( fileA, i+1, dataA[i]) )
                            print('{:>30}:{} - {}'.format( fileB, i+1, dataB[i]) )
                    elif ( i < len(dataA) ):
                        print('\n{:>}:{} - {}'.format( fileA, i+1, dataA[i]) )
                        print('{:>30}:{} - '.format( fileB, i+1 ) )
                    else:
                        print('\n{:>30}:{} - '.format( fileA, i+1 ) )
                        print('{:>30}:{} - {}'.format( fileB, i+1, dataB[i]) )
                    i = i + 1
        else:
            i = 1
            for lineA in dataA:
                equalContent = False
                for lineB in dataB:
                    if ( lineA == lineB ):
                        equalContent = True
                        break
                if ( not equalContent ):
                    if ( verbose ):
                        print('{}:{} - {} not found in {}'.format( fileA, i, lineA, fileB) )
                    break
                i = i + 1
            if ( equalContent ):
                i = 1
                for lineB in dataB:
                    equalContent = False
                    for lineA in dataA:
                        if ( lineA == lineB ):
                            equalContent = True
                            break
                    if ( not equalContent ):
                        if ( verbose ):
                            print('{}:{} - {} not found in {}'.format( fileB, i, lineB, fileA) )
                        break
                    i = i + 1

    if ( not equalContent ):
        print("\n[{}] Files {} and {} don't match".format(fail,fileA, fileB))

        if ( verbose ):
            print('Got file {}:'.format( fileA ) )
            i = 1
            for line in dataA:
                print('   {:>3}: {}'.format( i, line ) )
                i = i + 1
            print('Expected file {}:'.format( fileB ) )
            i = 1
            for line in dataB:
                print('   {:>3}: {}'.format( i, line ) )
                i = i + 1
    return equalContent


def compare_lists( listA, listB, skip_blanks = True, verbose = False, exact = True, trim = True ):
    """
    Compares two list of strings. The comparison may eliminate leading and
    trailing blanks and/or empty strings before comparison. The order of
    elements in the lists is not considered (i.e. lists are sorted before
    comparion). The comparison may consider either equal elements (A == B)
    or the presence of element A in string B (A in B).

    Parameters
    ----------
    listA : list of str
        List A
    listB : list of str
        List B
    exact : logical [optional]
        If set to True (default) the routine will check if the elements in list
        A match exactly the elements in list B. Otherwise the routine will check
        if the elements of list A are present (in) the elements of list B
    skip_blanks : logical [optional]
        If set to True (default) blank elements will be skipped in the comparison
    trim : logical [optional]
        If set to True (default) elements will be trimmed (remove leading and
        trailing blanks) before comparison
    verbose : logical [optional]
        If set to True prints and the lists are different the routine will print
        the two lists. The default is never to print the lists.

    Returns
    -------
    compare_lists : logical
        Returns true if the two lists match, false otherwise
    """

    dataA = listA.copy()
    dataB = listB.copy()

    if ( trim ):
        dataA = [ s.strip() for s in dataA ]
        dataB = [ s.strip() for s in dataB ]

    if ( skip_blanks ):
        dataA = [ s for s in dataA if s != '' ]
        dataB = [ s for s in dataB if s != '' ]

    if ( len(dataA) == len(dataB)):
        dataA.sort()
        dataB.sort()
        if ( exact ):
            equal = ( dataA == dataB )
        else:
            equal = True
            i = 0
            for b in dataB:
                if ( not dataA[i] in b):
                    equal = False
                    break
                i = i + 1
    else:
        equal = False

    if ( not equal ):
        print( "\n{} Lists don't match".format(fail))

        if ( verbose ):
            print( "Expected: ", listA )
            print( "Got:      ", listB )
            print()

    return equal

def cleanup( files ):
    """Deletes file in list

    Parameters
    ----------
    files : list of str | str
        File or List of files to be deleted
    """
    if ( isinstance( files, str ) ):
        if ( os.path.exists(files) ):
            os.remove( files )
    else:
        for f in files:
            if ( os.path.exists(f) ):
                os.remove( f )

def eval_msg( testPassed, key, test = None ):
    """Prints evaluation message

    Parameters
    ----------
    testPassed : logical
        Set to true if test is working properly
    key : str
        Question number
    test : str or list of str [optional]
        Description of test to print in case of failure
    """
    if ( testPassed ):
        print('[{}] {}\t{}'.format( ok, key, enunciado[key] ))
    else:
        if ( test ):
            if ( isinstance( test, str ) ):
                msg = test
            else:
                msg = ' '.join(test)
            print('[{}] Test was <{}>\t{}'.format( fail, key, msg ))
        print('[{}] {}\t{}'.format( fail, key, enunciado[key] ))
        if ( stoponerror ):
            print('Testing stopped because --stoponerror was specified' )
            sys.exit() # Temporário para parar a cada erro

def run_process( scr, input = None, timeout = 1, printerror = True ):
    """
    Runs a process and returns the associated CompletedProcess object

    The process has a maximum time of `timeout` seconds to execute, otherwise
    the command returns False.

    Parameters
    ----------
    scr : list
        Command to be run. Actual command invocation is the first list
        member, additional command line parameters are set in the
        remainder list elements
    input : list [optional]
        Input text to be sent to stdin, defaults to None
    timeout : int [optional]
        Timeout time, defaults to 1 (seconds)
    printerror : boolean [optional]
        The routine will store (extend) stderr output to variable userErrorLog
        If true, this will also print stderr output with a warning message

    Returns
    -------
    process : CompletedProcess | False
        Returns the associated CompletedProcess object on success, and
        False on timeout.
    """
    try:
        proc = subprocess.run( scr, input = input, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeout )
    except subprocess.TimeoutExpired:
        print( "{}\t{}\t(*error*) Timeout after {} second(s)".format(fail, scr[0],timeout) )
    except OSError as err:
        print( "{}\t{}\t (*error*) {}".format(fail, scr[0], err.strerror ) )
        if ( err.errno == 8 ):
            print( "{}\t{}\t (*error*) Possibly missing she-bang (e.g. #!/bin/bash)".format(fail, scr[0] ) )
    except Exception as inst:
        print( "{}\t{}\t (*error*) {}".format(fail, scr[0], type(inst) ) )
    else:
        if ( proc.stderr ):
            userErrorLog.clear()
            userErrorLog.extend( proc.stderr.decode("utf-8").split('\n') )
            if (printerror):
                error = proc.stderr.decode("utf-8").split('\n')
                if ( error[0] ):
                    print("\n{}  Output on user stderr from <{}>:".format(warn,' '.join(scr)))
                    for l in error:
                        if (l):
                            print( "{}\t{}".format(warn,l ))
                    print()
        return proc
    return False

def check_proc_out( scr, output, input = None, timeout = 1, printerror = True, verbose = True ):
    """
    Runs a process and checks if the stdout output matches the supplied values

    The process has a maximum time of `timeout` seconds to execute, otherwise
    the command returns False.

    Parameters
    ----------
    scr : list
        Command to be run. Actual command invocation is the first list
        member, additional command line parameters are set in the
        remainder list elements
    output : str
        Expected output
    input : list [optional]
        Input text to be sent to stdin, defaults to None
    timeout : int [optional]
        Timeout time, defaults to 1 (seconds)
    printerror : boolean [optional]
        The routine will store (extend) stderr output to variable userErrorLog
        If true, this will also print stderr output with a warning message

    Returns
    -------
    check : logical
        Returns true if the required `output` is in the process stdout
    """

    try:
        proc = subprocess.run( scr, input = input, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=timeout )

        if ( proc.stderr ):
            userErrorLog.clear()
            userErrorLog.extend( proc.stderr.decode("utf-8").split('\n') )
            if (printerror):
                error = proc.stderr.decode("utf-8").split('\n')
                if ( error[0] ):
                    print("\n{}  Output on user stderr from <{}>:".format(warn,' '.join(scr)))
                    for l in error:
                        if (l):
                            print( "{}\t{}".format(warn,l ))
                    print()

        if ( output in proc.stdout ):
            # print( "[{}]\t{}\t(*success*) got {} expected {}".format(ok,scr[0], proc.stdout, output) )
            result = True
        else:
            print( "[{}]\t{}\t(*error*) got {} expected {}".format(fail,scr[0], proc.stdout, output) )
            result = False

    except subprocess.TimeoutExpired:
        print( "[{}]\t{}\t(*error*) Timeout after {} second(s)".format(fail, scr[0],timeout) )
        result = False

    except OSError as err:
        print( "{}\t{}\t (*error*) {}".format(fail, scr[0], err.strerror ) )
        if ( err.errno == 8 ):
            print( "{}\t{}\t (*error*) Possibly missing she-bang (e.g. #!/bin/bash)".format(fail, scr[0] ) )
        result = False

    return result



##########################################################################
# Crete Input files from test data
#
###########################################################################

def create_test_files():
    """Creates test files
    """
    create_file( 'passageiros.txt', passageiros )
    create_file( 'voos.txt', voos )
    create_file( 'relatorio_reservas.txt', relatorio )

def cleanup_test_files():
    """Removes test files
    """
    cleanup ( ['passageiros.txt','voos.txt','relatorio_reservas.txt'])

##########################################################################
# Parte 1 - regista_passageiro.sh
#
##########################################################################

def test_regista_passageiro( t ) :
    """Testa o script 'regista_passageiro.sh'

    Alíneas S1.*

    Parameters
    ----------
    t : str
        Source directory
    """
    scr = t + "/regista_passageiro.sh"

    print( "\nS1 - regista_passageiro.sh...")

    if (not os.path.exists( scr )):
        print( "S1. (*error*) {} not found {}".format(scr,fail) )
        return

    cleanup( "passageiros.txt" )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S1.1.1}' ),
        'S1.1.1a', test = scr )

    test = [ scr, 'A' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.1.1}' ),
        'S1.1.1b', test = test )

    test = [ scr, 'A', 'B' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.1.1}' ),
        'S1.1.1c', test = test )

    test = [ scr, 'A', 'B', 'C', 'D', 'E' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.1.1}' ),
        'S1.1.1d', test = test )

    test = [ scr, 'David Gabriel NomeExtra', '12qwaszx', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.1.2}' ),
        'S1.1.2a', test = test )

    test = [ scr, 'David Gabrie', '12qwaszx', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.1.2}' ),
        'S1.1.2b', test = test )

    test = [ scr, 'avid Gabriel', '12qwaszx', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.1.2}' ),
        'S1.1.2c', test = test )

    test = [ scr, 'David Gabriel', '12qwaszx', 'a' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.1.3}' ),
        'S1.1.3', test = test )

    test = [ scr, 'David Gabriel', '12qwaszx', '10', 'absd' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.1.4}' ),
        'S1.1.4', test = test )

    # 1.2

    cleanup( "passageiros.txt" )

    test = [ scr, 'David Gabriel', '12qwaszx', '10', '234580880' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.2.1}' ),
        'S1.2.1', test = test )

    eval_msg( os.path.exists( "passageiros.txt" ), 'S1.2.2', test = test )

    test = [ scr, 'Ricardo Fonseca', 'senha', '10', '123456789' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.2.3}' ),
        'S1.2.3', test = test )

    cleanup( "passageiros.txt" )

    test = [ scr, 'Ricardo Fonseca', 'senha', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.2.4}' ),
        'S1.2.4', test = test )

    cleanup( "passageiros.txt" )

    test = [ scr, 'Ricardo Fonseca', 'senha', '10', '123456789' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {S1.2.5}' ),
        'S1.2.5', test = test )

    cleanup( "passageiros.txt" )

    test = [ scr, 'Joao Baptista Goncalves', 'senha', '10', '123456789' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {S1.2.6} [joao.goncalves@iscteflight.pt]' ),
        'S1.2.6', test = test )

    cleanup( "passageiros.txt" )

    # 1.2.7
    create_file( 'passageiros.txt', passageiros )

    newuser= "rfonseca:123456789:Ricardo Fonseca:ricardo.fonseca@iscteflight.pt:senha:10"

    test = [ scr, 'Ricardo Fonseca', 'senha', '10', '123456789' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {S1.2.7}' ),
        'S1.2.7', test = test )

    create_file( 'passageiros.eval',
                passageiros + (newuser,) )

    eval_msg( compare_files( "passageiros.txt", "passageiros.eval", verbose = True ),
             'S1.2.7b', test = test )

    cleanup( ["passageiros.eval","passageiros.txt"] )

    # 1.3
    create_file( 'passageiros.txt', passageiros )

    test = [ scr, 'David Gabriel', 'bad-pass', '10' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S1.3.1}' ),
        'S1.3.1', test = test )

    cleanup( "passageiros.txt" )

    # 1.3.2
    create_file( 'passageiros.txt', passageiros )

    test = [ scr, 'David Gabriel', '12qwaszx', '10' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {S1.3.2} [10010]' ),
        'S1.3.2a', test = test )

    update=list(passageiros)
    update[1] = "dlsgl:275067109:David Gabriel:david.gabriel@iscteflight.pt:12qwaszx:10010"
    create_file( 'passageiros.eval', update )

    eval_msg( compare_files( "passageiros.txt", "passageiros.eval", verbose = True ),
             'S1.3.2b', test = test )

    cleanup( ["passageiros.eval","passageiros.txt"] )

    # 1.4
    # this should be done automatically
    sorted = [ update[0], update[1], update[2] ]
    create_file( 'passageiros-saldos-ordenados.eval', sorted )

    eval_msg( compare_files( "passageiros-saldos-ordenados.txt", "passageiros-saldos-ordenados.eval", verbose = True ),
             'S1.4.1', test = test )

    cleanup( ["passageiros-saldos-ordenados.eval","passageiros-saldos-ordenados.txt"] )

##########################################################################
# Parte 2 - compra.sh
#
##########################################################################

def test_compra_bilhete( t ):
    """Testa o script 'compra_bilhete.sh'

    Alíneas S2.*

    Parameters
    ----------
    t : str
        Source directory
    """
    scr = t + "/compra_bilhete.sh"

    print( "\nS2 - compra_bilhete.sh...")

    if (not os.path.exists( scr )):
        print( "S2. (*error*) {} not found {}".format(scr,fail) )
        return


    cleanup_test_files()

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.1.1}' ),
        'S2.1.1a', test = scr )

    create_file( "voos.txt", voos )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.1.1}' ),
        'S2.1.1b', test = scr )

    create_file( "passageiros.txt", passageiros )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.1.2}',
        input = b"xyx\n" ),
        'S2.1.2a', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.1.2}',
        input = b"Londres\n" ),
        'S2.1.2b', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.1.2}',
        input = b"Londres\n" ),
        'S2.1.2b', test = scr )

    eval_msg( check_proc_out( [ scr ],
        b'@SUCCESS {S2.1.2} [Lisboa]\n1.Lisboa para Dubai, 2024-05-23, Partida:08h00, Pre\xc3\xa7o: 320, Dispon\xc3\xadveis:35 lugares\n2.Lisboa para Terceira, 2024-03-12, Partida:22h00, Pre\xc3\xa7o: 340, Dispon\xc3\xadveis:15 lugares\n0.Sair\n',
        input = b"Lisboa\n0\n" ),
        'S2.1.3a', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.1.3}',
        input = b"Lisboa\n5\n" ),
        'S2.1.3b', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.1.4}',
        input = b"Lisboa\n2\nrpafa\n" ),
        'S2.1.4', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.1.5}',
        input = b"Lisboa\n2\ndlsgl\nsenha\n" ),
        'S2.1.5', test = scr )

    update=list(passageiros)
    update.append("rfonseca:123456789:Ricardo Fonseca:ricardo.fonseca@iscteflight.pt:abc123:1")
    create_file( 'passageiros.txt', update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S2.2.1}',
        input = b"Lisboa\n2\nrfonseca\nabc123\n" ),
        'S2.2.1', test = scr )

    create_file( 'passageiros.txt', passageiros )
    eval_msg( check_proc_out( [ scr ],
         b'@SUCCESS {S2.2.2} [9660]',
        input = b"Lisboa\n2\ndlsgl\n12qwaszx\n" ),
        'S2.2.2a', test = scr )

    update=list(passageiros)
    update[1]="dlsgl:275067109:David Gabriel:david.gabriel@iscteflight.pt:12qwaszx:9660"

    create_file( 'passageiros.eval', update )

    eval_msg( compare_files( "passageiros.txt", "passageiros.eval", verbose = True ),
             'S2.2.2b', test = scr )

    cleanup( "passageiros.eval" )

    create_file( 'passageiros.txt', passageiros )
    create_file( 'voos.txt', voos )

    eval_msg( check_proc_out( [ scr ],
         b'@SUCCESS {S2.2.3}',
        input = b"Lisboa\n2\ndlsgl\n12qwaszx\n" ),
        'S2.2.3a', test = scr )

    update=list(voos)
    update[6]="PT5030:Lisboa:Terceira:2024-03-12:22h00:340:175:14"

    create_file( 'voos.eval', update )

    eval_msg( compare_files( "voos.txt", "voos.eval", verbose = True ),
             'S2.2.3b', test = scr )

    cleanup( "voos.eval" )

    create_file( 'passageiros.txt', passageiros )
    create_file( 'voos.txt', voos )

    create_file( 'relatorio_reservas.txt', relatorio )

    today=datetime.datetime.now().strftime("%Y-%m-%d")
    now=datetime.datetime.now().strftime("%Hh%M")

    eval_msg( check_proc_out( [ scr ],
         b'@SUCCESS {S2.2.4}',
        input = b"Lisboa\n2\ndlsgl\n12qwaszx\n" ),
        'S2.2.4a', test = scr )

    update=list(relatorio)
    update.append("12093:PT5030:Lisboa:Terceira:340:dlsgl:{}:{}".format(today,now))
    create_file( 'relatorio_reservas.eval', update )

    eval_msg( compare_files( "relatorio_reservas.txt", "relatorio_reservas.eval", verbose = True ),
             'S2.2.4b', test = scr )

    cleanup( ["passageiros.txt", "passageiros.txt.bak", "relatorio_reservas.txt", "relatorio_reservas.eval" ] )
    cleanup( ["voos.txt", "voos.txt.bak"] )

##########################################################################
# Parte 3 - estado_voos.sh
#
##########################################################################

def test_estado_voos( t ):
    """Testa o script 'estado_voos.sh'

    Alíneas S3.*

    Parameters
    ----------
    t : str
        Source directory
    """

    scr = t + "/estado_voos.sh"

    userErrorLog = []

    print( "\nS3 - estado_voos.sh...")


    if (not os.path.exists( scr )):
        print( "S3. (*error*) {} not found {}".format(scr,fail) )
        return

    # 3.1.1
    cleanup_test_files()

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {S3.1.1}') ,
        'S3.1.1', test = scr )

    # 3.1.2
    update=list(voos)
    #"PR3201:Londres:Nova York:2024-03-09:10h00:1500:285:0"
    update[2]=":Londres:Nova York:2024-03-09:10h00:1500:285:0"
    create_file( "voos.txt", update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {'+bytearray(update[2],"utf-8")+b'}') ,
        'S3.1.2a', test = scr )

    update[2]="PR3201::Nova York:2024-03-09:10h00:1500:285:0"
    create_file( "voos.txt", update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {'+bytearray(update[2],"utf-8")+b'}') ,
        'S3.1.2b', test = scr )

    update[2]="PR3201:Londres::2024-03-09:10h00:1500:285:0"
    create_file( "voos.txt", update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {'+bytearray(update[2],"utf-8")+b'}') ,
        'S3.1.2c', test = scr )

    update[2]="PR3201:Londres:Nova York:data:10h00:1500:285:0"
    create_file( "voos.txt", update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {'+bytearray(update[2],"utf-8")+b'}') ,
        'S3.1.2d', test = scr )

    update[2]="PR3201:Londres:Nova York:2024-03-09:hora:1500:285:0"
    create_file( "voos.txt", update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {'+bytearray(update[2],"utf-8")+b'}') ,
        'S3.1.2e', test = scr )

    update[2]="PR3201:Londres:Nova York:2024-03-09:10h00:abc:285:0"
    create_file( "voos.txt", update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {'+bytearray(update[2],"utf-8")+b'}') ,
        'S3.1.2f', test = scr )

    update[2]="PR3201:Londres:Nova York:2024-03-09:10h00:1500:def:0"
    create_file( "voos.txt", update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {'+bytearray(update[2],"utf-8")+b'}') ,
        'S3.1.2g', test = scr )

    update[2]="PR3201:Londres:Nova York:2024-03-09:10h00:1500:285:x"
    create_file( "voos.txt", update )

    eval_msg( check_proc_out( [ scr ],
         b'@ERROR {'+bytearray(update[2],"utf-8")+b'}') ,
        'S3.1.2h', test = scr )

    create_file( "voos.txt", voos )
    today=datetime.datetime.now().strftime("%Y-%m-%d")
    now=datetime.datetime.now().strftime("%Hh%M")

    eval_msg( check_proc_out( [ scr ],
         b'@SUCCESS {S3.2.1}') ,
        'S3.2.1a', test = scr )

    eval_msg( check_proc_out( [ scr ],
         b'@SUCCESS {S3.2.1}') ,
        'S3.2.1a', test = scr )

    html="""<html><head><meta charset="UTF-8"><title>IscteFlight: Lista de Voos Disponíveis</title></head>
<body><h1>Lista atualizada em {} {}</h1>
<h2>Voo: FR2101, De: Lisboa Para Dubai, Partida em 2024-05-23 08h00</h2>
<ul>
<li><b>Lotação:</b> 210 Lugares</li>
<li><b>Lugares Disponíveis:</b> 35 Lugares</li>
<li><b>Lugares Ocupados:</b> 175 Lugares</li>
</ul>
<h2>Voo: FR5102, De: Dubai Para Bangkok, Partida em 2024-03-11 12h30</h2>
<ul>
<li><b>Lotação:</b> 220 Lugares</li>
<li><b>Lugares Disponíveis:</b> 220 Lugares</li>
<li><b>Lugares Ocupados:</b> 0 Lugares</li>
</ul>
<h2>Voo: PT6202, De: Tóquio Para Singapura, Partida em 2024-03-10 16h45</h2>
<ul>
<li><b>Lotação:</b> 290 Lugares</li>
<li><b>Lugares Disponíveis:</b> 10 Lugares</li>
<li><b>Lugares Ocupados:</b> 280 Lugares</li>
</ul>
<h2>Voo: FR2301, De: Dubai Para Sydney, Partida em 2024-11-04 17h30</h2>
<ul>
<li><b>Lotação:</b> 315 Lugares</li>
<li><b>Lugares Disponíveis:</b> 150 Lugares</li>
<li><b>Lugares Ocupados:</b> 165 Lugares</li>
</ul>
<h2>Voo: PT5030, De: Lisboa Para Terceira, Partida em 2024-03-12 22h00</h2>
<ul>
<li><b>Lotação:</b> 175 Lugares</li>
<li><b>Lugares Disponíveis:</b> 15 Lugares</li>
<li><b>Lugares Ocupados:</b> 160 Lugares</li>
</ul>
</body></html>""".format(today,now)

    print(html, file=open('voos_disponiveis.eval', 'w'))

    eval_msg( compare_files( "voos_disponiveis.html", "voos_disponiveis.eval", verbose = True ),
             'S3.2.1b', test = scr )

    cleanup( ["voos.txt", "voos_disponiveis.html", "voos_disponiveis.eval"] )

##########################################################################
# Parte 4 - stats.sh
#
##########################################################################

def test_stats( t ):
    """Testa o script 'stats.sh'

    Alíneas 4.*

    Parameters
    ----------
    t : str
        Source directory
    """

    scr = t + "/stats.sh"

    print( "\nS4 - stats.sh...")

    if (not os.path.exists( scr )):
        print( "S4. (*error*) {} not found {}".format(scr,fail) )
        return

    test = [ scr ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S4.1.1}' ),
        'S4.1.1a', test = test )

    test = [ scr, 'A' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S4.1.1}' ),
        'S4.1.1b', test = test )

    test = [ scr, 'top' ]
    eval_msg( check_proc_out( test,
         b'@ERROR {S4.1.1}' ),
        'S4.1.1c', test = test )

    create_file( "passageiros.txt", passageiros )
    create_file( "relatorio_reservas.txt", relatorio )

    test = [ scr, 'passageiros' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {S4.2.1}' ),
        'S4.2.1a', test = test )

    stats="""Fábio Cardoso: 3 reservas; 3790€
David Gabriel: 2 reservas; 2150€
Nuno Garrido: 1 reservas; 450€"""

    print( stats, file=open('stats.eval', 'w'))

    eval_msg( compare_files( "stats.txt", "stats.eval", verbose = True ),
             'S4.2.1a', test = scr )

    cleanup( ["stats.txt", "stats.eval"] )

    create_file( "voos.txt", voos )

    test = [ scr, 'top', '3' ]
    eval_msg( check_proc_out( test,
         b'@SUCCESS {S4.2.2}' ),
        'S4.2.2a', test = test )

    stats="""PT5000: 3000€
PT6202: 1700€
FR5102: 1350€"""

    print( stats, file=open('stats.eval', 'w'))

    eval_msg( compare_files( "stats.txt", "stats.eval", verbose = True ),
             'S4.2.2b', test = scr )

    cleanup( ["stats.txt", "stats.eval"] )

    cleanup( ["passageiros.txt", "relatorio_reservas.txt", "voos.txt"])

##########################################################################
# Parte 5 - menu.sh
#
##########################################################################

def test_menu( t ):
    """Testa o script 'menu.sh'

    Alíneas S5.*

    Parameters
    ----------
    t : str
        Source directory
    """

    scr = t + "/menu.sh"

    print( "\nS5 - menu.sh...")

    if (not os.path.exists( scr )):
        print( "S5. (*error*) {} not found {}".format(scr,fail) )
        return

    # Alínea 5.1
    opts = bytes('0\n','ascii')
    proc = run_process( [scr], input = opts )
    if ( proc ):
        eval_msg( True, 'S5.1a')
    else:
        eval_msg( False, 'S5.1a')
        print('{} {}'.format( "Menu fails to exit, manual correction needed",fail))
        return

    menu="""1: Regista/Atualiza saldo do passageiro
2: Reserva/Compra de bilhetes
3: Atualiza Estado dos voos
4: Estatísticas - Passageiros
5: Estatísticas - Top Voos + Rentáveis
0: Sair
"""

    if ( bytearray(menu,"utf-8") in proc.stdout ):
        eval_msg( True, 'S5.1b')
    else:
        eval_msg( False, 'S5.1b')
        print('[{}] {}'.format(fail, "O texto do menu não está de acordo com o enunciado"))

    # 5.2
    opts = bytes('1\nRicardo Fonseca\npass\n2\n3\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] regista_passageiro.sh 'Ricardo Fonseca' 'pass' '3' '2'\n", input = opts ),
        'S5.2.2.1' )

    opts = bytes('2\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] compra_bilhete.sh ok\n", input = opts ),
        'S5.2.2.2' )

    opts = bytes('3\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] estado_voos.sh ok\n", input = opts ),
        'S5.2.2.3' )

    opts = bytes('4\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] ok stats.sh passageiros\n", input = opts ),
        'S5.2.2.4' )

    opts = bytes('5\n2\n\n0\n','ascii')
    eval_msg( check_proc_out( [ scr ],
        b"[*SO*] ok stats.sh top '2'\n", input = opts ),
        'S5.2.2.5' )


##########################################################################
# Main
#
##########################################################################

parser = argparse.ArgumentParser( description="Evaluate test problem",
    usage = '%(prog)s [-h] [-g] [-l] [-d] [-e] <source>' )

parser.add_argument( "-g","--generate", dest="generate", action='store_true', \
                    help = "Generate test files")

parser.add_argument( "-l","--cleanup", dest="cleanup", action='store_true', \
                    help = "Cleanup test files")

parser.add_argument( "-d","--debug", dest="debug", action='store_true', \
                    help = "Shows DEBUG information")

parser.add_argument( "-e","--stoponerror", dest="stoponerror", action='store_true', \
                    help = "Stop validation on any error")

parser.add_argument( 'Source',
    metavar='source', nargs = '?', default = '..',
    type =str,
    help='Directory with source files to evaluate' )

# Parse command line arguments
args = parser.parse_args()

# If requested generate test files and exit
if ( args.generate ):
    print("Generating test files...")
    create_test_files()
    print("done.")
    exit(0)

# If requested cleanup test files and exit
if ( args.cleanup ):
    print("Cleaning up test files...")
    cleanup_test_files()
    print("done.")
    exit(0)

# If requested stoponerror, set variable to stop if any error is found (to avoid the parsing of the whole script on initial errors)
if ( args.stoponerror ):
    print("Testing will stop immediately if any error is found")
    stoponerror = True

if ( args.Source is None ):
    print("Source directory not specified")
    parser.print_usage()
    exit(1)

# Get test directory
testdir = args.Source

# Check if directory exists
if (not os.path.exists( testdir )):
    print('(*error*) Directory "{}" does not exist'.format(testdir))
    exit(1)

# Check if it is current directory
if ( os.path.samefile(".", testdir)):
    print('(*error*) Source must not be local (.) directory')
    exit(2)

# Temporary: Message to Students
# print('The Validator Script is under construction and will be made available soon, please wait...')
# exit(0)

# Check for additional scripts
scripts = [
    "regista_passageiro.sh",
    "compra_bilhete.sh",
    "estado_voos.sh",
    "stats.sh"
    ]

for s in scripts:
    if ( not os.path.exists(s)):
        print('(*error*) "{}" script not found'.format(s))
        exit(3)

questions = [*enunciado]
pauta = []

for i in questions:
    grade[i] = 0.0

# Run tests
print("Evaluating directory '{}'...".format(testdir))

test_regista_passageiro( testdir )
test_compra_bilhete( testdir )
test_estado_voos( testdir )
test_stats( testdir )
test_menu( testdir )

print("\ndone.")
