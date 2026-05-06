#!/usr/bin/python3

from udjat import logger, agent

class SampleAgent(agent):
	def __init__(self,settings):
		print("Initializing sample agent")
		super().__init__(settings)

def agent_factory(settings):
	#print(dir(agent))
	logger.info("Agent factory method was called")
	agent = SampleAgent(settings)
