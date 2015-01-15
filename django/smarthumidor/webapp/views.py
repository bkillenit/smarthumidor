from django.shortcuts import render
from django.http import HttpResponse
from django.template import RequestContext, loader
import json
import httplib2


# Create your views here.
def index(request):
	h = httplib2.Http(".cache")
	resp, content = h.request("http://example.org/", "GET")
	template = loader.get_template('webapp/index.html')

	return render(request, 'webapp/index.html')