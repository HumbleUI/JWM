#! /usr/bin/env python3
import argparse, base64, build, clean, common, glob, json, os, platform, re, subprocess, sys, time, urllib

def main():
  os.chdir(os.path.dirname(__file__) + '/..')
  parser = argparse.ArgumentParser()
  parser.add_argument('--ref', required=True)
  (args, _) = parser.parse_known_args()
  rev = re.fullmatch("refs/[^/]+/([^/]+)", args.ref).group(1)

  ossrh_username = os.getenv('OSSRH_USERNAME')
  ossrh_password = os.getenv('OSSRH_PASSWORD')

  mvn = "mvn.cmd" if common.system == "windows" else "mvn"
  mvn_settings = [
    '--settings', 'deploy/settings.xml',
    '-Dossrh.username=' + ossrh_username,
    '-Dossrh.password=' + ossrh_password,
    '-Durl=https://s01.oss.sonatype.org/service/local/staging/deploy/maven2/',
    '-DrepositoryId=ossrh',
    '-DpomFile=target/maven/META-INF/maven/io.github.humbleui.jwm/jwm/pom.xml'
  ]

  print('Deploying jwm-' + rev + ".jar")
  subprocess.check_call(
    [mvn, 'gpg:sign-and-deploy-file'] + \
    mvn_settings + \
    ['-Dfile=target/jwm-' + rev + '.jar'])

  print('Deploying jwm-' + rev + "-sources.jar")
  subprocess.check_call(
    [mvn, 'gpg:sign-and-deploy-file'] + \
    mvn_settings + \
    ['-Dfile=target/jwm-' + rev + '-sources.jar',
     '-Dclassifier=sources'])

  print('Deploying jwm-' + rev + "-javadoc.jar")
  subprocess.check_call(
    [mvn, 'gpg:sign-and-deploy-file'] + \
    mvn_settings + \
    ['-Dfile=target/jwm-' + rev + '-javadoc.jar',
     '-Dclassifier=javadoc'])

  headers = {
    'Accept': 'application/json',
    'Authorization': 'Basic ' + base64.b64encode((ossrh_username + ":" + ossrh_password).encode('utf-8')).decode('utf-8'),
    'Content-Type': 'application/json',
  }

  def fetch(path, data = None):
    req = urllib.request.Request('https://s01.oss.sonatype.org/service/local/staging' + path,
                               headers=headers,
                               data = json.dumps(data).encode('utf-8') if data else None)
    resp = urllib.request.urlopen(req).read().decode('utf-8')
    # print(' ', path, "->", resp)
    return json.loads(resp) if resp else None

  print('Finding staging repo')
  resp = fetch('/profile_repositories')
  repo_id = resp['data'][0]["repositoryId"]
  
  print('Closing repo', repo_id)
  resp = fetch('/bulk/close', data = {"data": {"description": "", "stagedRepositoryIds": [repo_id]}})

  while True:
    print('Checking repo', repo_id, 'status')
    resp = fetch('/repository/' + repo_id + '/activity')
    close_events = [e for e in resp if e['name'] == 'close']
    close_events = close_events[0]['events'] if close_events else []
    fail_events = [e for e in close_events if e['name'] == 'ruleFailed']
    if fail_events:
      print(fail_events)
      return 1

    if close_events and close_events[-1]['name'] == 'repositoryClosed':
      break

    time.sleep(0.5)

  print('Releasing staging repo', repo_id)
  resp = fetch('/bulk/promote', data = {"data": {
              "autoDropAfterRelease": True,
              "description": "",
              "stagedRepositoryIds":[repo_id]
        }})

  return 0

if __name__ == "__main__":
  sys.exit(main())
