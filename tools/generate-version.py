#!/usr/bin/env python

import os
import hashlib
import subprocess
import sys

def md5(fname):
    hash_md5 = hashlib.md5()
    with open(fname, "r") as f:
        content = f.readlines()
    content__encode = ''.join(content).encode('utf-8')
    # print (content__encode)
    hash_md5.update(content__encode)
    return hash_md5.hexdigest()

def main():
    prefix = 'LIB'
    if len(sys.argv) > 1:
        output_dir = sys.argv[1]
        if len(sys.argv) > 2:
            prefix = sys.argv[2]
    else:
        output_dir = os.path.join('versions', 'include')
    print ('Generating version_generated_header.h in ' + output_dir)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    generated_header_filename = os.path.join(output_dir, 'version_generated_header.h')
    old_md5 = ''
    if os.path.exists(generated_header_filename):
        old_md5 = md5(generated_header_filename)

    header_value = ''
    header_value += '// This is a generated header from generate-version.py\n'
    # generated_header.write('// This is a generated header from generate-version.py\n')
    cwd = os.getcwd()
    print ('PYTHON CWD: ' + cwd)
    git_count = subprocess.check_output(['git', 'rev-list', '--count', 'HEAD'], cwd = cwd).decode(sys.stdout.encoding)
    # print(git_count)
    git_describe = subprocess.check_output(['git', 'describe', '--always'], cwd = cwd).decode(sys.stdout.encoding)
    git_last = subprocess.check_output(['git', 'rev-parse', '--verify', 'HEAD'], cwd = cwd).decode(sys.stdout.encoding)
    ver = git_count.strip().zfill(7) + '-' + git_last.strip()[:7]
    header_value += '#define ' + prefix + '_VERSION "' + ver + '-' + git_describe.strip() + '"\n'
    # generated_header.write('#define ' + prefix + '_VERSION "' + git_count.strip() + '-' + git_describe.strip() + '"\n')
    header_value += '#define ' + prefix + '_VERSION_LAST_GUID "' + git_last.strip() + '"\n'
    # generated_header.write('#define ' + prefix + '_VERSION_LAST_GUID "' + git_last.strip() + '"\n')
    git_date = subprocess.check_output(['git', 'show', '-s', '--format=%cd', git_last.strip()], cwd = cwd).decode(sys.stdout.encoding)
    header_value += '#define ' + prefix + '_VERSION_LAST_DATE "' + git_date.strip() + '"\n'

    hash_md5 = hashlib.md5()
    hash_md5.update(header_value.encode('utf-8'))
    new_md5 = hash_md5.hexdigest()

    print ('New md5: ' + new_md5)
    print ('Old md5: ' + old_md5)
    if new_md5 != old_md5:
        with open(generated_header_filename, 'w') as generated_header:
            generated_header.write(header_value)
    # generated_header.write('#define ' + prefix + '_VERSION_LAST_DATE "' + git_date.strip() + '"\n')


if __name__ == "__main__":
    main()