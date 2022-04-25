import unittest
import requests as r

def _get(url):
	ret = r.get(url)
	return ret.status_code

class TestRequests(unittest.TestCase):
	def test_requests(self):
		url = "http://localhost:8081/"
		tests = [
			("index.html", 200),
			("php/env.php", 200),
			("php/w3_post_form.php", 200),
			("php/w3_get_welcome.php?name=Test&email=greg@example.com", 200),
			("filedoesnotexist.html", 404),
			("directorydoesnotexist/", 404),
			("test1/../", 200),
		]

		for test in tests:
			self.assertEqual(_get(url + test[0]), test[1])
	
	def test_redirects(self):
		url = "http://localhost:8081/"
		tests = [
			("redirect", 301),
			("redirect/index.html", 301),
		]

		for test in tests:
			self.assertEqual(_get(url + test[0]), test[1])

	def test_get_query_string(self):
		url = "http://localhost:8081/php/w3_get_welcome.php?name=Test&email=greg@example.com"
		ret = r.get(url)
		print(ret.content)
		self.assertTrue(ret.content.decode('utf-8').find("Welcome Test"))
		self.assertTrue(ret.content.decode('utf-8').find("Your email address is: greg@example.com"))

	def test_post_form(self):
		url = "http://localhost:8081/php/w3_post_welcome.php"
		data = {
			"name": "bob",
			"email": "bob@example.com"
		}
		ret = r.post(url, data=data)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode('utf-8').find("Welcome bob"))
		self.assertTrue(ret.content.decode('utf-8').find("Your email address is: bob@example.com"))

if __name__ == '__main__':
	unittest.main()