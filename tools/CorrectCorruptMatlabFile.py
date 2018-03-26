# CorrectCorruptMatlabFile.py

__doc__ = """CorrectCorruptMatlabFile.py

Tries to un-corrupt a corrupt MatlabFile as written by a ToFile block in TwinCAT.

Usage:
python CorrectCorruptMatlabFile.py <filename.mat> [<number>]

<filename.mat>: The file to process
<number>: Optional, normally the script can find this out itself.
   The number of elements (doubles) for each sample. Note that this 
   is one more than the number of elements you count in the simulink model because it
   also stores the time.

A new file, with name <filename_corrected.mat> is created"""

import sys, os
import shutil
import struct

# Temporary to execute it from PyZo, override command line arguments:
# sys.argv=["CorrectCorruptMatlabFile.py","D:\\Profiles\\oortg\\Desktop\\achillesprobleem\\Offending\\MeasAngularVelocity_part0.mat"]

def GetRelevantDataFromMatFile(filename, overrideNElementsPerSample=None):
    # If nElementsPerSample was not given as input argument, it reads it from 
    # the file (which is the default and usually best case)
    # Then, it calculates the number of samples, nSamples, based on the length
    # of the file (i.e., it ignores the nSamples value stored in the file header)
    fileSize = os.path.getsize(filename)
    f = open(filename,'rb')
    
    # Check number of elemnts per sample
    if overrideNElementsPerSample==None:
        # Then try to determine it automatically
        f.seek(4) # Jump to number of elements per sample and read it.
        nElementsPerSample = struct.unpack("i",f.read(4))[0]
        if nElementsPerSample == 0:
            print("""ERROR: Cannot find out the number of elements automatically. Try again,
        and enter the number of elements per sample manually.
        Or, invoke without any arguments for help.""")
            f.close()
            exit()
        nElementsPerSampleSource="determined automatically"
    else:
        nElementsPerSample =overrideNElementsPerSample
        nElementsPerSampleSource="as given by user"
        
    
    #  Check variable name length and calculate number of samples
    f.seek(16) # Jump to the variablename length
    varnamelen = struct.unpack("i",f.read(4))[0]
    datalen = fileSize -20 -varnamelen # 20 is header length
    
    nSamplesDouble = datalen/8/nElementsPerSample
    nSamples = datalen//8//nElementsPerSample
    f.close()
    y = dict(nElementsPerSample= nElementsPerSample,
        nElementsPerSampleSource= nElementsPerSampleSource,
        varnamelen= varnamelen,
        datalen= datalen,
        nSamplesDouble= nSamplesDouble,
        nSamples= nSamples)
    return y

def main(args):
    
    ## Check arguments
    if len(args)==1: # No arguments
        print(__doc__)
        exit()
        
    if args[1].upper() in ['HELP','-HELP','--HELP','/HELP','-H','/H','--H', '?','/?', '-?']:
        print(__doc__)
        exit()
    
    if not (len(args) in [2,3]):
        print  ("ERROR: expected 1 or 2 arguments. Invoke without any arguments to get help.")
        exit()
    
    # Check first argument    
    fileName = args[1]
    if not isinstance(fileName, str):
        print("ERROR: First argument should be a file name.\nInvoke without any arguments to get help.")
        exit()
    
    # Check second argument
    if len(args)==3:
        nElementsPerSampleStr = args[2]
        try:
            nElementsPerSample= int(nElementsPerSampleStr)
        except ValueError:
            print("""ERROR: Second argument should be an integer indicating the number of elements
        stored per sample (one more than the number of elements you count in the Simulink model).
        Invoke without any arguments to get help.""")
            exit()
    else:
        nElementsPerSample = None
    
    if not os.path.isfile(fileName):
        print("ERROR: file %s does not exist."%fileName)
        exit()
        
    
    # If we're here, everything should be fine
    temp = os.path.splitext(fileName)
    
    outputFileName = temp[0]+'_corrected'+temp[1]
    print("Input  file name:", fileName)
    
    ## Copy the file
    shutil.copyfile(fileName, outputFileName)
    # We can now work solely on the output file
    
    # Get nElementsPerSample etc.
    r=GetRelevantDataFromMatFile(outputFileName, nElementsPerSample)

   
    print("Number of doubles per sample (%s): %i"%(r['nElementsPerSampleSource'],r['nElementsPerSample']))
    print("Number of samples in file:", r['nSamples'], '(',r['nSamplesDouble'],')')
    
    
    ## Modify the file
    f = open(outputFileName,'r+b')
    # Write the required data
    f.seek(4)
    f.write(struct.pack("ii",r['nElementsPerSample'], r['nSamples']))
    
    # Close and truncate file
    f.seek(20+r['varnamelen']+r['nSamples']*r['nElementsPerSample']*8) # Move pointer to the supposed end of the file
    f.truncate()
    f.close()
    
    print("Output file name:",outputFileName)
    
    print("Done.")

## Execute main if run from command line
if __name__ == "__main__":
    main(sys.argv)
