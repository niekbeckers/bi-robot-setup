using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using TwinCAT.Ads;

namespace AppControlDisplay
{

    public enum TCADSTypes : int
    {
        SHORT = 0,
        INT,
        SINGLE,
        DOUBLE,
        DOUBLEARRAY,
        BOOL,
        BOOLARRAY,
        SHORTARRAY,
        INTARRAY,
        SINGLEARRAY
    };

    public class TCADSTask
    {
        // Properties
        protected object _variable;
        protected int _varHandle;
        protected TcAdsClient _tcClient;

        protected IDictionary<Type, TCADSTypes> _typeDict = new Dictionary<Type, TCADSTypes>();

        public TCADSTask()
        {
            // Fill type dictionary
            _typeDict.Add(typeof(short), TCADSTypes.SHORT);
            _typeDict.Add(typeof(int), TCADSTypes.INT);
            _typeDict.Add(typeof(Single), TCADSTypes.SINGLE);
            _typeDict.Add(typeof(double), TCADSTypes.DOUBLE);
            _typeDict.Add(typeof(double[]), TCADSTypes.DOUBLEARRAY);
            _typeDict.Add(typeof(bool[]), TCADSTypes.BOOLARRAY);
            _typeDict.Add(typeof(short[]), TCADSTypes.SHORTARRAY);
            _typeDict.Add(typeof(Single[]), TCADSTypes.SINGLEARRAY);
            _typeDict.Add(typeof(int[]), TCADSTypes.INTARRAY);

        }

        public TCADSTask(TcAdsClient tcClientIn, int varHandleIn, object objIn) : this()
        {
            _tcClient = tcClientIn;
            _variable = objIn;
            _varHandle = varHandleIn;
        }

        public virtual void Work()
        {

        }

        public void GetTypeSizeLength(ref int size, ref int len)
        {
            len = 1;
            switch (_typeDict[_variable.GetType()])
            {
                case TCADSTypes.SHORT: size = sizeof(short); break;
                case TCADSTypes.INT: size = sizeof(int); break;
                case TCADSTypes.SINGLE: size = sizeof(Single); break;
                case TCADSTypes.DOUBLE: size = sizeof(double); break;
                case TCADSTypes.DOUBLEARRAY: size = sizeof(double); len = ((double[])_variable).Length; break;
                case TCADSTypes.BOOLARRAY: size = sizeof(bool); len = ((bool[])_variable).Length;  break;
                case TCADSTypes.SHORTARRAY: size = sizeof(short); len = ((short[])_variable).Length; break;
                case TCADSTypes.INTARRAY: size = sizeof(int); len = ((int[])_variable).Length; break;
                case TCADSTypes.SINGLEARRAY: size = sizeof(Single); len = ((Single[])_variable).Length; break;
            }
        }
    }

    public class TCADSTaskWrite: TCADSTask
    {
        public TCADSTaskWrite(TcAdsClient tcClientIn, int varHandleIn, object objIn) :
            base(tcClientIn, varHandleIn, objIn) { }

        public override void Work()
        {
            // Determine size (and length in case of array) of variable
            int size = 0;
            int len = 1;
            GetTypeSizeLength(ref size, ref len);

            // Create AdsStream, BinaryWriter to write to TcAdsClient
            AdsStream dataStream = new AdsStream(size * len);
            BinaryWriter binWrite = new BinaryWriter(dataStream);
            dataStream.Position = 0;

            try
            {
                // Write to ADS Client
                switch (_typeDict[_variable.GetType()])
                {
                    case TCADSTypes.SHORT: binWrite.Write((short)_variable); break;
                    case TCADSTypes.INT: binWrite.Write((int)_variable); break;
                    case TCADSTypes.SINGLE: binWrite.Write((Single)_variable); break;
                    case TCADSTypes.DOUBLE: binWrite.Write((double)_variable); break;
                    case TCADSTypes.DOUBLEARRAY: foreach (double d in (double[])_variable) { binWrite.Write(d); } break;
                    case TCADSTypes.BOOL: binWrite.Write((bool)_variable); break;
                }

                _tcClient.Write(_varHandle, dataStream);
            }
            catch (Exception err)
            {
                Console.WriteLine(err.ToString());
            }
        }
    }

    public class TCADSTaskRead : TCADSTask
    {

        public TCADSTaskRead(TcAdsClient tcClientIn, int varHandleIn, ref short[] objIn)
            : base(tcClientIn, varHandleIn, objIn) { }

        public TCADSTaskRead(TcAdsClient tcClientIn, int varHandleIn, ref int[]objIn)
            : base(tcClientIn, varHandleIn, objIn) { }
 
        public TCADSTaskRead(TcAdsClient tcClientIn, int varHandleIn, ref Single[] objIn)
            : base(tcClientIn, varHandleIn, objIn) { }

        public TCADSTaskRead(TcAdsClient tcClientIn, int varHandleIn, ref double[] objIn)
            : base(tcClientIn, varHandleIn, objIn) { }

        public TCADSTaskRead(TcAdsClient tcClientIn, int varHandleIn, ref bool[] objIn)
            : base(tcClientIn, varHandleIn, objIn) { }

        public override void Work()
        {
            // Determine size (and length in case of array) of variable
            int size = 0;
            int len = 1;
            GetTypeSizeLength(ref size, ref len);

            // Create AdsStream, BinaryWriter to write to TcAdsClient
            AdsStream dataStream = new AdsStream(size * len);
            BinaryReader binRead = new BinaryReader(dataStream);
            dataStream.Position = 0;

            try
            {
                int numBytes = _tcClient.Read(_varHandle, dataStream); // Read, get number of bytes

                // Parse into Array 
                for (int i = 0; i < len; i++ )
                {
                    switch (_typeDict[_variable.GetType()])
                    {
                        case TCADSTypes.SHORTARRAY: ((short[])_variable)[i] = (short)binRead.ReadInt16(); break;
                        case TCADSTypes.INTARRAY: ((int[])_variable)[i] = (int)binRead.ReadInt32(); break;
                        case TCADSTypes.SINGLEARRAY: ((Single[])_variable)[i] = binRead.ReadSingle(); break;
                        case TCADSTypes.DOUBLEARRAY: ((double[])_variable)[i] = binRead.ReadDouble(); break;
                        case TCADSTypes.BOOLARRAY: ((bool[])_variable)[i] = binRead.ReadBoolean(); break;
                    }
                }

            }
            catch (Exception err)
            {
                Console.WriteLine(err.ToString());
            } 
        }
    }
}
