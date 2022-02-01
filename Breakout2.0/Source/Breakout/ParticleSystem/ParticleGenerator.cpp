#include "pch.h"
#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int nParticles)
	:shader(shader), texture(texture), nr_particles(nParticles)
{
	init();
}

void ParticleGenerator::Update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
	// add new particles
	for (unsigned int i = 0; i < newParticles; i++)
	{
		int freeParticle = firstUnusedParticle();
		respawnParticle(particles[freeParticle], object, offset);
	}

	// update all particles
	for (unsigned int i = 0; i < nr_particles; i++ )
	{
		Particle& p = particles[i];
		p.Life -= dt; // ooh so that it's life is determined by the time
		if (p.Life > 0.0f)
		{
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 2.5f;
		}
	}
}

void ParticleGenerator::Draw()
{
	shader.Use();
	// blend function to give it a glow effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (Particle& particle : particles)
	{
		if (particle.Life <= 0.0f)
			continue;
		shader.SetVector2f("offset", particle.Position);
		shader.SetVector4f("color", particle.Color);
		texture.Bind();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
	//reset the binding mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void ParticleGenerator::init()
{
	unsigned int VBO;

	float particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	for (unsigned int i = 0; i < nr_particles; i++)
	{
		particles.push_back(Particle());
	}

	glDeleteBuffers(1, &VBO);
}

unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
	for (unsigned int i = lastUsedParticle; i < nr_particles; i++)
	{
		if (particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	for (unsigned int i = 0; i < lastUsedParticle; ++i)
	{
		if (particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	// if all particles are taken override the first one
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.01f;
}
