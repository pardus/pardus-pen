# Define the command to extract the version
cmd = ['bash', '-c', 'head -n 1 changelog | sed "s/.*(//g;s/).*//g"']

# Create a custom target
version = run_command(cmd, check:true).stdout().strip()
add_project_arguments('-DVERSION='+version, language: 'cpp')