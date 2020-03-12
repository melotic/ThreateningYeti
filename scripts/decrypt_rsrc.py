import pefile
import blowfish
import sys

if len(sys.argv) != 2:
    print ("[-] usage: decrypt_rsrc.py <lockdown_path>")
    sys.exit(-1)

print("[*] Loading Lockdown PE File")
pe = pefile.PE(sys.argv[1])
enc_resource = None

for entry in pe.DIRECTORY_ENTRY_RESOURCE.entries:
    if str(entry.name) == str("RBINARY"):
        enc_resource = entry.directory.entries[0].directory.entries[0]

if enc_resource == None:
    print("[-] Couldn't find RBINARY")
    sys.exit(-1)

size = enc_resource.data.struct.Size
offset = enc_resource.data.struct.OffsetToData

print("[*] Found RBINARY resource, size={} offset={}".format(size, offset))

# extract data, first 4 bytes is the length of data
data = pe.get_memory_mapped_image()[offset+4:offset+size]

print("[*] Decrypting data")
bf_cipher = blowfish.Cipher(b"nQ6sSow6w5plZ72t")
data_decrypted = b"".join(bf_cipher.decrypt_ecb(data))

print("[*] Saving decrypted data")
with open('ldb_strings.bin', 'wb') as f:
    f.write(data_decrypted)

print("[*] Generating C Array with blacklisted exe's\n")
# process data in 2.06.01 is [0x1C341:0x4172b], lets generalize
data_process = data_decrypted[0x1C000:]

index=0
length=len(data_process)
blacklisted = []

while index < length:
    i = data_process.find(b'.exe', index)
    if i == -1:
        break
    
    chars = ""
    j = i
    while True:
        curr_char = data_process[j]
        if curr_char == 0:
            break
        chars = chr(curr_char) + chars
        j = j - 1

    chars = chars + "exe"
    blacklisted.append(chars)
    index = i + 1

# https://stackoverflow.com/questions/53808694/how-do-i-format-a-python-list-as-an-initialized-c-array
def to_c_array(values, ctype="static const wchar_t*", name="blacklisted_exes", colcount=6):
    values = ['L"' + v + '"' for v in values]
    rows = [values[i:i+colcount] for i in range(0, len(values), colcount)]
    body = ',\n    '.join([', '.join(r) for r in rows])
    return '{} {}[] = {{\n    {}}};'.format(ctype, name, body)

print(to_c_array(blacklisted) + "\n")