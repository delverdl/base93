using System.Buffers.Binary;
using System.Linq;
using System.Text;

namespace base93
{
    public class CBase93
    {
        private string? _value;

        private void SetStringStream(string? s)
        {
            if (s == null) return;
            SetBytesStream(Encoding.UTF8.GetBytes(s));
        }

        private void SetBytesStream(Span<byte> bytes)
        {
            byte[] lb = bytes.ToArray();
            int nPos = lb.Length;
            int missing = nPos & 7; //Distance to previous 8-multiple
            LinkedList<ulong> l = new();

            if (missing > 0) //Resize and fill array as needed
            {
                missing = 8 - missing; //Missing bytes to complete a 64-bit number
                Array.Resize(ref lb, missing + nPos);
                Array.Fill(lb, (byte)0, nPos, missing);
            }
            for (int i = 0; i < lb.Length; i += 8)
            {
                ulong bc = BitConverter.ToUInt64(lb, i);

                l.AddLast(bc);
            }
            SetUlongStream(l.ToArray());
        }

        private static string Pad(string s)
        {
            while (s.Length % 10 != 0) s = '!' + s;
            return s;
        }

        private void SetUlongStream(Span<ulong> numbers)
        {
            _value = string.Empty;
            foreach (ulong ul in numbers)
            {
                ulong bc = BitConverter.IsLittleEndian ? BinaryPrimitives.ReverseEndianness(ul) : ul;

                _value += FromUlong(bc);
            }
        }

        public CBase93() { }

        public CBase93(string? value)
        {
            SetStringStream(value);
        }

        public CBase93(Span<ulong> numbers)
        {
            SetUlongStream((Span<ulong>)numbers);
        }

        public CBase93(Span<byte> bytes)
        {
            SetBytesStream((Span<byte>)bytes);
        }

        public string? Value { get => ToString(); set => SetStringStream(value); }

        public string? Encoded { get => _value; set => _value = value; }

        public new string ToString()
        {
            string r = _value ?? string.Empty;

            if (!string.IsNullOrEmpty(r))
            {
                string str = Pad(r);

                r = string.Empty;
                for (int i = 0; i < str.Length; i += 10)
                {
                    var ul = ToULong(str.Substring(i, 10));

                    if (BitConverter.IsLittleEndian)
                        ul = BinaryPrimitives.ReverseEndianness(ul);
                    r += Encoding.UTF8.GetString(BitConverter.GetBytes(ul));
                }
            }
            return r;
        }

        public static implicit operator string(CBase93 v)
        {
            return v.ToString();
        }

        public static implicit operator CBase93(string v)
        {
            return new CBase93(v);
        }

        public static implicit operator CBase93(ulong v)
        {
            return new CBase93(new ulong[] { v });
        }

        public static implicit operator CBase93(byte[] v)
        {
            return new CBase93(v);
        }

        public static string FromUlong(ulong n)
        {
            string r = string.Empty;

            while (n >= 93)
            {
                r = (char)((n % 93) + '!') + r;
                n /= 93;
            }
            r = (char)(n + '!') + r;
            return Pad(r);
        }

        public static ulong ToULong(string? b)
        {
            if (string.IsNullOrEmpty(b) || b.Length > 10) return 0;

            ulong nLevel = 1;
            ulong r = 0;
            string tmp = b;

            foreach (var c in tmp.Reverse())
            {
                if (c < '!' || c > '~') return 0;
                r += ((ulong)c - (ulong)'!') * nLevel;
                nLevel *= 93;
            }
            return r;
        }
    }
}