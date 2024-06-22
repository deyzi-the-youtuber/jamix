import subprocess
import os

if __name__ == "__main__":
    file_path = "./include/jamix/version.h"

    # Check if the file exists
    if os.path.exists(file_path):
        # Delete the file if it exists
        os.remove(file_path)
        print(f"Deleted existing file: {file_path}")

    # Open the file in write mode
    file = open(file_path, "w")

    file.write("#ifndef __VERSION_H\n")
    file.write("#define __VERSION_H\n\n")
    process = subprocess.Popen(["whoami"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    whoami_user, err = process.communicate()
    process = subprocess.Popen(["hostname"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    hostname_user, err = process.communicate()
    file.write(f"char * jamix_boot_banner = \"Jamix kernel version %s compiled by {whoami_user.decode('utf-8').strip()}@{hostname_user.decode('utf-8').strip()} on %s\\n\";\n\n")
    file.write("#endif /* __VERSION_H */")
