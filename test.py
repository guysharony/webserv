import unittest
import requests as r

def _get(url, redirect=True):
	ret = r.get(url, allow_redirects=redirect)
	return ret.status_code

class TestIndexLinks(unittest.TestCase):
	def test_index(self):
		url = "http://localhost:8081/index.html"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode().find("Welcome to my web server") >= 0)

	def test_root(self):
		url = "http://localhost:8081/"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode().find("Welcome to my web server") >= 0)

	def test_cgi(self):
		url = "http://localhost:8081/php/env.php"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode().find("HTTP_") >= 0)
	
	def test_port8082(self):
		url = "http://localhost:8082/"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode().find("Welcome to my web server") >= 0)

	def test_get_form(self):
		url = "http://localhost:8081/php/w3_get_form.php"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode().find("form") >= 0)
		self.assertTrue(ret.content.decode().find("name") >= 0)
		self.assertTrue(ret.content.decode().find("email") >= 0)
		url = "http://localhost:8081/php/w3_get_welcome.php?name=Test&email=greg@example.com"
		ret = r.get(url)
		self.assertTrue(ret.content.decode('utf-8').find("Welcome Test") >= 0)
		self.assertTrue(ret.content.decode('utf-8').find("Your email address is: greg@example.com") >= 0)

	def test_post_form(self):
		url = "http://localhost:8081/php/w3_post_form.php"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode().find("form") >= 0)
		self.assertTrue(ret.content.decode().find("name") >= 0)
		self.assertTrue(ret.content.decode().find("email") >= 0)
		url = "http://localhost:8081/php/w3_post_welcome.php"
		data = {
			"name": "bob",
			"email": "bob@example.com"
		}
		ret = r.post(url, data=data)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode('utf-8').find("Welcome bob") >= 0)
		self.assertTrue(ret.content.decode('utf-8').find("Your email address is: bob@example.com") >= 0)

	def test_upload_form(self):
		url = "http://localhost:8081/php/w3_upload_form.php"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode().find("form") >= 0)
		self.assertTrue(ret.content.decode().find("file") >= 0)
		url = "http://localhost:8081/php/w3_upload.php"
		with open('www/static/delete/del.html', 'rb') as upload_file:
			files = {
				'fileToUpload': upload_file,
			}
			data = {

			}
			ret = r.post(url, files=files, data=data)
		self.assertEqual(ret.status_code, 200)
		url = "http://localhost:8081/php/uploads/del.html"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode().find("Delete me") >= 0)
		ret = r.delete(url)
		self.assertEqual(ret.status_code, 204)
		url = "http://localhost:8081/php/uploads/del.html"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 404)

	def test_403_error(self):
		url = "http://localhost:8081/auto-index-off/"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 403)
		self.assertTrue(ret.content.decode().find("403 Forbidden") >= 0)
		self.assertTrue(ret.content.decode().find("This request cannot be authorized") >= 0)

	def test_404_error(self):
		url = "http://localhost:8081/fakefile.html/"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 404)
		self.assertTrue(ret.content.decode().find("Error 404") >= 0)
		self.assertTrue(ret.content.decode().find("The page you are looking for can't be found.") >= 0)

	@unittest.skip("Test crashes server")
	def test_405_error(self):
		url = "http://localhost:8081/post_body/"
		try:
			ret = r.get(url)
			self.assertEqual(ret.status_code, 405)
			self.assertTrue(ret.content.decode().find("405 method not allowed") >= 0)
			self.assertTrue(ret.content.decode().find("The method requested is not allowed") >= 0)
		except Exception as e:
			print("error")
			raise e		


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
			self.assertEqual(_get(url + test[0], redirect=False), test[1])

	@unittest.skip("Not implemented for files that do not exist")
	def test_create_file(self):
		url = "http://localhost:8081/delete/newfiletodelete"
		files = {
			'fileToUpload': open('test.py', 'rb'),
		}
		data = {

		}
		ret = r.post(url, files=files, data=data)
		self.assertEqual(ret.status_code, 201)

	def test_get_query_string(self):
		url = "http://localhost:8081/php/w3_get_welcome.php?name=Test&email=greg@example.com"
		ret = r.get(url)
		self.assertTrue(ret.content.decode('utf-8').find("Welcome Test") >= 0)
		self.assertTrue(ret.content.decode('utf-8').find("Your email address is: greg@example.com") >= 0)

	def test_post_form(self):
		url = "http://localhost:8081/php/w3_post_welcome.php"
		data = {
			"name": "bob",
			"email": "bob@example.com"
		}
		ret = r.post(url, data=data)
		self.assertEqual(ret.status_code, 200)
		self.assertTrue(ret.content.decode('utf-8').find("Welcome bob") >= 0)
		self.assertTrue(ret.content.decode('utf-8').find("Your email address is: bob@example.com") >= 0)
	
	def test_get_cgi_file_does_not_exist(self):
		url = "http://localhost:8081/php/asdgasg.php"
		ret = r.get(url)
		self.assertEqual(ret.status_code, 404)
		self.assertTrue(ret.content.decode('utf-8').find("404") >= 0)

	def test_request_with_invalid_host(self):
		url = "http://localhost:8081/hello.htm"
		headers = {
			"Host": "www.tutorialspoint.com",
		}
		ret = r.get(url, headers=headers)
		self.assertEqual(ret.status_code, 404)
		url = "http://localhost:8081/index.html"
		ret = r.get(url, headers=headers)
		self.assertEqual(ret.status_code, 200)


if __name__ == '__main__':
	unittest.main()
